CC := /usr/bin/gcc

all: clean | task1

task1:
	$(CC) -o search.o src/main.c

clean:
	rm -rf *.o
