#include "gifwrapper.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "stb_image.h"
#include "gifenc.h"
#include "tinydir.h"

struct gifwrapper_s {
    uint8_t **gifs;
    uint8_t palette[256 * 3];
    int colors;
    int depth;
    int first;
    int last;
    unsigned int delay;
    int x;
    int y;
    unsigned int total;
    char **image_filenames;
};

typedef struct gifwrapper_s gifwrapper;

static int begins_with(char *s, char *t) {
    return strncasecmp(s,t,strlen(t)) == 0;
}

static int ends_with(char *s, char *t) {
    int se = strlen(s);
    int te = strlen(t);
    if(te > se) return 0;
    return strcasecmp(s + se - te, t) == 0;
}

static unsigned int load_folder(gifwrapper *gif, const char *path) {
    tinydir_dir dir;
    unsigned int i = 0;
    unsigned int j = 0;
    if(tinydir_open_sorted(&dir,path) == -1) return 0;
    gif->image_filenames = (char **)malloc(sizeof(char *)*dir.n_files);
    if(gif->image_filenames == NULL) return 0;
    memset(gif->image_filenames,0,sizeof(char *) * dir.n_files);
    for(i=0; i < dir.n_files; i++) {
        tinydir_file file;
        tinydir_readfile_n(&dir,&file,i);
        if( (!begins_with(file.name,".")) && (ends_with(file.name,".bmp") || ends_with(file.name,".png"))) {
            gif->image_filenames[j] = (char *)malloc(sizeof(char) * strlen(file.path) + 1);
            if(gif->image_filenames[j] == NULL) {
                return 0;
            }
            strcpy(gif->image_filenames[j],file.path);
            fprintf(stderr,"Image found: %s\n",gif->image_filenames[j]);
            fflush(stderr);
            j++;
        }
    }
    gif->total = j;
    return j;
}


static int setup_loop(gifwrapper *gif) {
    unsigned int i = 1;
    if(i==gif->total) {
        gif->delay = 0;
        return 1;
    }

    /* find the first non-duplicate frame */
    do {
        if(memcmp(gif->gifs[i-1],gif->gifs[i],gif->x * gif->y) != 0) break;
        i++;
    } while(i<gif->total);
    if(i == gif->total) {
        gif->first = 0;
        gif->last = gif->total;
        return 1;
    }
    gif->first = i;

    while(i<gif->total) {
        if(memcmp(gif->gifs[gif->first-1],gif->gifs[i],gif->x * gif ->y) == 0) break;
        i++;
    }
    if(i == gif->total) {
        gif->last = gif->total;
        return 1;
    }
    while(i<gif->total) {
        if(memcmp(gif->gifs[gif->first],gif->gifs[i],gif->x * gif->y) == 0) break;
        i++;
    }
    gif->last = i;
    return 1;
}

static int encode_gif(gifwrapper *gif, const char *output) {
    ge_GIF *g = NULL;

    fprintf(stderr,"Writing %s, looping from %d -> %d\n",output,gif->first,gif->last);
    g = ge_new_gif(output, gif->x, gif->y, gif->palette, gif->depth, 0);
    if(g == NULL) {
        fprintf(stderr,"error opening gif for output\n");
        return 1;
    }
    int i = 0;
    int delay;

    while(i<gif->last) {
        delay = gif->delay;

        while(
          (i+1 < gif->last) &&
          (memcmp(gif->gifs[i],gif->gifs[i+1],gif->x * gif->y) == 0)
        ){
            delay += gif->delay;
            i++;
        }

        memcpy(g->frame,gif->gifs[i],gif->x * gif->y);
        ge_add_frame(g,delay);

        i++;
    }
    ge_close_gif(g);
    fprintf(stderr,"Done!");
    return 0;
}

static int power(int n, int e) {
    int r = 1;
    while(e--) {
        r *= n;
    }
    return r;
}

static int str_sort(const void *a, const void *b) {
    const char **ia = (const char **)a;
    const char **ib = (const char **)b;
    return strcmp(*ia, *ib);
}

static int load_images(gifwrapper *gif) {
    int i = 0;
    int j = 0;
    stbi_uc *image = NULL;
    int x = 0;
    int y = 0;
    int c = 0;
    int p = 0;

    qsort(gif->image_filenames,gif->total,sizeof(char *),str_sort);

    gif->gifs = (uint8_t **)realloc(gif->gifs,sizeof(uint8_t *) * gif->total);
    if(gif->gifs == NULL) {
        fprintf(stderr,"out of memory\n");
        return 0;
    }

    while(gif->image_filenames[i] != NULL) {
      fprintf(stderr,"Loading %s\n",gif->image_filenames[i]);
      fflush(stderr);
      image = stbi_load(gif->image_filenames[i], &x, &y, &c, 3);
      if(image == NULL) {
          fprintf(stderr,"something went wrong: %s\n", stbi_failure_reason());
          return 0;
      }

      if(i == 0) {
          gif->x = x;
          gif->y = y;
      } else {
          if(gif->x != x || gif->y != y) {
              fprintf(stderr,"error: %s has different dimensions from the first image\n",gif->image_filenames[i]);
              fprintf(stderr,"%dx%d vs %dx%d\n",gif->x,gif->y,x,y);
              return 0;
          }
      }

      gif->gifs[i] = (uint8_t *)malloc(sizeof(uint8_t) * x * y);
      if(gif->gifs[i] == NULL) {
          fprintf(stderr,"out of memory\n");
          return 0;
      }

      j = 0;
      while( j < (x * y) ) {
          /* see if we've seen this pallete before */
          p = 0;
          while(p<gif->colors) {

              if(gif->palette[p*3] == image[j*3] && gif->palette[(p*3)+1] == image[(j*3)+1] && gif->palette[(p*3)+2] == image[(j*3)+2]) {
                  break;
              }
              p++;
          }
          if(p == gif->colors) {
              gif->colors = p + 1;
              if(gif->colors > 255) {
                  fprintf(stderr,"too many colors and I can't quantize, shit.\n");
                  return 0;
              }
              gif->palette[(p*3)] = image[j*3];
              gif->palette[(p*3)+1] = image[(j*3)+1];
              gif->palette[(p*3)+2] = image[(j*3)+2];
          }
          gif->gifs[i][j] = p;
          j++;
      }
      stbi_image_free(image);
      i++;
    }
    return 1;
}

/* main entrypoint for the GUI */
int gifwrapper_doit( unsigned int delay, const char *output, const char *folder) {
    gifwrapper _gif;
    gifwrapper *gif = &_gif;

    gif->gifs = NULL;
    gif->colors = 0;
    gif->depth = 1;
    gif->first = 0;
    gif->last = 0;
    gif->delay = 2;
    if(delay > 0) gif->delay = delay;
    gif->x = -1;
    gif->y = -1;
    gif->image_filenames = NULL;
    gif->total = 0;
    memset(gif->palette,0,256 *3);

    if(load_folder(gif,folder) == 0) {
        return 1;
    }

    if(!load_images(gif)) {
        return 1;
    }

    gif->colors++;
    while(power(2,gif->depth) < gif->colors) {
        gif->depth++;
    }

    setup_loop(gif);

    if(encode_gif(gif,output)) {
        return 1;
    }
    return 0;
}
