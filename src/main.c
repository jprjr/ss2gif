#include <stdio.h>
#include <string.h>
#include "gifwrapper.h"

int main(int argc, char *argv[]) {
    char *output = NULL;
    char *input = NULL;
    unsigned int delay = 2;

    if(argc < 2) {
        fprintf(stderr,"Usage: %s -d delay -o output folder",argv[0]);
        return 1;
    }

    argv++;
    argc--;

    while(argc > 0) {
        if(strcmp(*argv,"-d") == 0) {
            argv++;
            argc--;
            if(!sscanf(*argv,"%u",&delay)) {
                fprintf(stderr,"error: -d (delay) must be a number\n");
                return 1;
            }
        }
        else if(strcmp(*argv,"-o") == 0) {
            argv++;
            argc--;
            output = *argv;
        }
        else {
            if(input == NULL) {
                input = *argv;
            } else {
                fprintf(stderr,"error: input folder already given\n");
                return 1;
            }
        }
        argv++; argc--;
    }

    if(output == NULL) {
        fprintf(stderr,"no output file specified\n");
        return 1;
    }

    if(input == NULL) {
        fprintf(stderr,"no input folder specified\n");
        return 1;
    }

    return gifwrapper_doit(delay,output,input);
}
