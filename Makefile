CC = gcc
CFLAGS = -Wall -Isrc/simlib

bin/bus: src/bus.c src/simlib/simlib.c
	mkdir -p bin
	$(CC) $(CFLAGS) src/bus.c src/simlib/simlib.c -o bin/bus -lm

run: bin/bus
	./bin/bus

docs: docs/laporan.tex src/bus.c data/bus.out
	cd docs && latexmk -pdf laporan.tex && latexmk -c laporan.tex

clean:
	rm -rf bin data/bus.out

.PHONY: run docs clean
