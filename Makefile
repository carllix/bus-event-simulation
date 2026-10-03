CC = gcc
CFLAGS = -Wall -Isrc/simlib

bin/bus: src/bus.c src/simlib/simlib.c
	mkdir -p bin
	$(CC) $(CFLAGS) src/bus.c src/simlib/simlib.c -o bin/bus -lm

run: bin/bus
	./bin/bus

clean:
	rm -rf bin data/bus.out