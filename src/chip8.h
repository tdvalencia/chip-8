#pragma once

#include <stdint.h>

typedef struct Chip8 {
    uint8_t     V[16];
    uint16_t    I, SP, PC;
    uint8_t     delay;
    uint8_t     sound;
    uint8_t     *memory;
    uint8_t     *screen; // memory[0xF00]
} Chip8;

Chip8* InitChip8(void);
int emulate_op(Chip8* cpu);