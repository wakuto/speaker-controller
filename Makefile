CC=gcc
CFLAGS=-lpulse-simple
OUT=speaker-controller

all: main.o

main.o: main.c
	$(CC) $(CFLAGS) main.c -o $(OUT)

clean:
	rm -f *.o $(OUT)
