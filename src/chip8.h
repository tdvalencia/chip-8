#pragma once

#include <stdint.h>

#define GFX_COLS 64
#define GFX_ROWS 32
#define GFX_SIZE (GFX_COLS * GFX_ROWS)

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
    uint8_t     vram[GFX_SIZE];
    uint8_t     key_state[KEY_SIZE];
    int         halt, draw_flag;
} Chip8;

Chip8* InitChip8(void);
void emulate_op(Chip8* cpu);