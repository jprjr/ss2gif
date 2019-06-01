.PHONY: all clean windows

CFLAGS = -Wall -Wextra -g

OBJS = src/gifwrapper.o src/gifenc.o src/stb_image.o

all: ss2gif

ss2gif: src/main.o $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm -f src/*.res src/*.o src/gui-lua.c src/gui-lua.h ss2gif ss2gif.exe
