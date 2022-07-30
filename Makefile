CC = gcc

SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/*.c, src/*.o, $(SRC))

INC  = -Ilib/SDL2/include
LINK = -Llib/SDL2/lib -lmingw32 -lSDL2main -lSDL2
FLAG = -O2 -Wall

all: chip8

chip8: $(OBJ)
	$(CC) $(INC) $(FLAG) $^ -o $@ $(LINK)

src/*.o: $(SRC)
	$(CC) -c $(FLAG) $^ $(INC) -o $@

run:
	./chip8.exe rom/outlaw.ch8

clean:
	rm *.o
