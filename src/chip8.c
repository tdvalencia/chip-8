#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "chip8.h"
#include "disassemble.h"
#include "font4x5.h"

Chip8* InitChip8(void) {
    Chip8* cpu = calloc(sizeof(Chip8), 1);
    
    cpu->memory = calloc(1024*4, 1);
    cpu->SP = 0xfa0;
    cpu->PC = 0x200;
    cpu->halt = 0;
    cpu->draw_flag = 1;

    memcpy(&cpu->memory[FONT_BASE], font4x5, FONT_SIZE);

    return cpu;
}

void Unimplemented(Chip8* cpu) {
    printf("Instruction not implemented: ");
    DisassembleCHIP8(cpu->memory, cpu->PC);
    printf("\n");
    exit(1);
}

void emulate_op(Chip8* cpu) {

    uint16_t opcode;
    uint8_t x, y, n;
    uint8_t nn;
    uint16_t nnn;

    opcode = cpu->memory[cpu->PC] << 8 | cpu->memory[cpu->PC + 1];
    x      = (opcode >> 8) & 0x000f;    // the lower 4 bits of the high byte
    y      = (opcode >> 4) & 0x000f;    // the upper 4 bits of the low byte
    n      = opcode & 0x000f;   // the lowest 4 bits
    nn     = opcode & 0x00ff;   // the lowest 8 bits
    nnn    = opcode & 0x0fff;   // the lowest 12 bits


    DisassembleCHIP8(cpu->memory, cpu->PC);
    printf("\n");

    switch (opcode & 0xf000) {
        case 0x0000: {
            switch (nn) {
                case 0x00e0: {    // CLS
                    memset(cpu->vram, 0, sizeof(uint8_t) * GFX_SIZE);
                    cpu->draw_flag = 1;
                    cpu->PC+=2;
                    break;
                } break;
                case 0x00ee: {    // RTS
                    cpu->SP--;
                    cpu->PC = cpu->stack[cpu->SP];
                    cpu->PC+=2;
                } break;
            }
        } break;

        case 0x1000: {             // JMP $NNN
            cpu->PC = nnn;
        } break;

        case 0x2000: {            // CALL $NNN
            cpu->stack[cpu->SP] = cpu->PC;
            cpu->SP++;
            cpu->PC = nnn;
        } break;

        case 0x3000: {           // SKIP.EQI Vx,$NN
            if (cpu->V[x] == nn)
                cpu->PC+=2;
            cpu->PC+=2;
        } break;

        case 0x4000: {             // SKIP.NEI Vx,$NN
            if (cpu->V[x] != nn)
                cpu->PC+=2;
            cpu->PC+=2;
        } break;

        case 0x5000: {             // SKIP.EQ Vx,Vy
            if (cpu->V[x] == cpu->V[y])
                cpu->PC+=2;
            cpu->PC+=2;
        } break;

        case 0x6000: {             // MVI Vx,$NN
            cpu->V[x] = nn;
            cpu->PC+=2;
        } break;

        case 0x7000: {             // ADI Vx,$NN
            cpu->V[x] += nn;
            cpu->PC+=2;
        } break;

        case 0x8000: {
            switch (n) {
                case 0x0: {       // MOV Vx,Vy
                    cpu->V[x] = cpu->V[y];
                } break;
                case 0x1: {       // OR Vx,Vy
                    cpu->V[x] = cpu->V[x] | cpu->V[y];
                } break;
                case 0x2: {       // AND Vx,Vy
                    cpu->V[x] = cpu->V[x] & cpu->V[y];
                } break;
                case 0x3: {       // XOR Vx,Vy
                    cpu->V[x] = cpu->V[x] ^ cpu->V[y];
                } break;
                case 0x4: {       // ADD Vx,Vy
                    cpu->V[0xf] = ((int) cpu->V[x] + (int) cpu->V[y]) > 255 ? 1 : 0;
                    cpu->V[x] = cpu->V[x] + cpu->V[y];
                } break;
                case 0x5: {       // SUB Vx,Vy
                    cpu->V[0xf] = (cpu->V[x] > cpu->V[y]) ? 1 : 0;
                    cpu->V[x] = cpu->V[x] - cpu->V[y];
                } break;
                case 0x6: {       // SHR Vx,1
                    cpu->V[0xf] = cpu->V[x] & 0x1;
                    cpu->V[x] = (cpu->V[x] >> 1);
                } break;
                case 0x7: {       // SUBB Vx,(Vy-Vx)
                    cpu->V[0xf] = (cpu->V[y] > cpu->V[x]) ? 1 : 0;
                    cpu->V[x] = cpu->V[y] - cpu->V[x];
                } break;
                case 0xe: {     // SHL Vx,1
                    cpu->V[0xf] = (cpu->V[x] >> 7) & 0x1;
                    cpu->V[x] = (cpu->V[x] << 1);
                } break;
            }
            cpu->PC+=2;
        } break;

        case 0x9000: {             // SKIP.NE Vx,Vy
            if (cpu->V[x] != cpu->V[y])
                cpu->PC+=2;
            cpu->PC+=2;
        } break;

        case 0xa000: {             // MVI I,$NNN
            cpu->I = nnn;
            cpu->PC+=2;
        } break;

        case 0xb000: {             // (JMP $NNN) + V0
            cpu->PC = nnn + cpu->V[0];
        } break;

        case 0xc000: {             // RAND Vx,$NN
            cpu->V[x] = (rand() % 256) & nn;
            cpu->PC+=2;
        } break;

        case 0xd000: {             // SPRITE Vx,Vy,$N
            unsigned row = cpu->V[y];
            unsigned col = cpu->V[x];

            cpu->V[0xf] = 0;
            for (unsigned byte_index = 0; byte_index < n; byte_index++) {
                uint8_t byte = cpu->memory[cpu->I + byte_index];

                for (unsigned bit_index = 0; bit_index < 8; bit_index++) {

                    int position = ((row + byte_index) % GFX_ROWS) * 
                            GFX_COLS + ((col + (7 - bit_index)) % GFX_COLS);

                    uint8_t bit = (byte >> bit_index) & 0x1;
                    uint8_t *pixelp = &cpu->vram[position];

                    if (bit == 1 && *pixelp == 1) cpu->V[0xf] = 1;

                    *pixelp = *pixelp ^ bit;
                }
            }
            cpu->draw_flag = 1;
            cpu->PC+=2;
        } break;
        
        case 0xe000: {
            switch (nn) {
                case 0x9e: {    // SKIP.KEY_Y 
                    cpu->PC += (cpu->key_state[cpu->V[x]]) ? 4 : 2;
                } break;
                case 0xa1: {    // SKIP.KEY_N
                    cpu->PC += (!cpu->key_state[cpu->V[x]]) ? 4 : 2;
                } break;
            }
        } break;
        
        case 0xf000: {
            switch (nn) {
                case 0x07: {    // MOV Vx,DELAY
                    cpu->V[x] = cpu->delay;
                    cpu->PC+=2;
                } break;
                case 0x0a: {    // WAITKEY Vx
                    for (int i = 0; i < KEY_SIZE; i++) {
                        if (cpu->key_state[i]) {
                            cpu->V[x] = i;
                            goto got_key_press;
                        }
                    }
                    return;
                    got_key_press:
                    cpu->PC+=2;
                } break;
                case 0x15: {    // MOV DELAY,Vx
                    cpu->delay = cpu->V[x];
                    cpu->PC+=2;
                } break;
                case 0x18: {    // MOV SOUND,Vx
                    cpu->sound = cpu->V[x];
                    cpu->PC+=2;
                } break;
                case 0x1e: {    // ADI I,Vx
                    cpu->V[0xf] = (cpu->I + cpu->V[x] > 0xfff) ? 1 : 0;
                    cpu->I = cpu->I + cpu->V[x];
                    cpu->PC+=2;
                } break;
                case 0x29: {
                    cpu->I = FONT_BASE + (cpu->V[x] * 5);
                    cpu->PC+=2;
                } break;
                case 0x33: {    // BCD MOV
                    cpu->memory[cpu->I]   = (cpu->V[x] % 1000) / 100; // hundreds place
                    cpu->memory[cpu->I+1] = (cpu->V[x] % 100) / 10;   // tens place
                    cpu->memory[cpu->I+2] = (cpu->V[x] % 10);         // ones place
                    cpu->PC+=2;
                } break;
                case 0x55: {    // REG_DUMP
                    for (int i = 0; i <= x; i++)
                        cpu->memory[cpu->I+i] = cpu->V[i];
                    cpu->I += (x+1);
                    cpu->PC+=2;
                } break;
                case 0x65: {    // REG_LOAD
                    for (int i = 0; i <= x; i++)
                        cpu->V[i] = cpu->memory[cpu->I+i];
                    cpu->I += (x+1);
                    cpu->PC+=2;
                } break;
            }
        } break;
    }
}

void chip8_tick(Chip8* cpu) {
    if (cpu->delay > 0) {
        cpu->delay--;
        if (cpu->delay == 0) {
            printf("second\n");
        }
    }
    if (cpu->sound > 0) {
        cpu->sound--;
        if (cpu->sound == 0) {
            printf("Sound\n");
        }
    }
}