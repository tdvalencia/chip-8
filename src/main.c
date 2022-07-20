#include <stdio.h>
#include <stdlib.h>

#include "chip8.h"

static int debug = 1;

void read_file_to_memory(Chip8* cpu, const char* filename, uint32_t offset) {
    FILE *f = fopen(filename, "rb");
    if (f == NULL) {
        printf("error: Couldn't open %s\n", filename);
        exit(1);
    }

    fseek(f, 0L, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0L, SEEK_SET);

    uint8_t *buffer = &cpu->memory[offset];
    
    fread(buffer, fsize, 1, f);
    fclose(f);
}

int main(int argc, char* argv[]) {

    char filename[] = "rom/test_opcode.ch8";

    Chip8* cpu = InitChip8();
    read_file_to_memory(cpu, filename, 0x200);

    printf("\nPC   B1 B2\n");
    printf("----------\n");
    while (1) {
        emulate_op(cpu);
    }

    return 0;
}