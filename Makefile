CC = gcc

SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/*.c, src/*.o, $(SRC))

INC  = -Ilib/SDL2/include
LINK = -Llib/SDL2/lib -lmingw32 -lSDL2main -lSDL2

all: chip8

chip8: $(OBJ)
	$(CC) $(INC) $^ -o $@ $(LINK)

src/*.o: $(SRC)
	$(CC) -c -Wall -O0 $^ $(INC) -o $@

run:
	./chip8.exe

clean:
	rm *.o