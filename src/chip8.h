#pragma once

#include <stdint.h>

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

#define REGISTERS 16
#define STACK_SIZE 16
#define KEY_SIZE 16

typedef struct Chip8 {
    uint8_t     V[REGISTERS];
    uint16_t    stack[STACK_SIZE];
    uint16_t    I, SP, PC;
    uint8_t     delay;
    uint8_t     sound;
    uint8_t     *memory;
    uint8_t     *screen; // memory[0xF00]
    uint8_t     key_state[KEY_SIZE];
    int         halt;
} Chip8;

Chip8* InitChip8(void);
int emulate_op(Chip8* cpu);