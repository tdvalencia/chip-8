CC = gcc

SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/*.c, src/*.o, $(SRC))

all: chip8

chip8: $(OBJ)
	$(CC) $^ -o $@

src/*.o: $(SRC)
	$(CC) -c -Wall -O0 $^ -o $@

run:
	./chip8.exe