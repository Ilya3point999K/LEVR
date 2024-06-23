LFLAGS = -lwinmm -lgdi32
install:
	gcc -std=c99 -O3 levrdemo.c $(LFLAGS) -o levrdemo