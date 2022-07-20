#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "chip8.h"
#include "disassemble.h"

Chip8* InitChip8(void) {
    Chip8* cpu = calloc(1, sizeof(Chip8));
    
    cpu->memory = calloc(1, 4096);
    cpu->screen = &cpu->memory[0xf00];
    cpu->SP = 0xfa0;
    cpu->PC = 0x200;

    return cpu;
}

void Unimplemented(Chip8* cpu) {
    printf("Instruction not implemented: ");
    DisassembleCHIP8(cpu->memory, cpu->PC);
    printf("\n");
    exit(1);
}

int emulate_op(Chip8* cpu) {
    uint8_t *code = &cpu->memory[cpu->PC];
    int firstnib = (*code & 0xf0) >> 4;

    DisassembleCHIP8(cpu->memory, cpu->PC);
    printf("\n");

    switch (firstnib) {
        case 0x0: Unimplemented(cpu); break;

        case 0x1: {        // JUMP $NNN
            uint16_t target = ((code[0]&0xf)<<8) | code[1];
            cpu->PC = target;
        } break;

        case 0x2: {
            cpu->SP -= 2;
            cpu->memory[cpu->SP] = ((cpu->PC+2) & 0xff00) >> 8;
            cpu->memory[cpu->SP+1] = (cpu->PC+2) & 0xff;
            cpu->PC = ((code[0]&0xf)<<8) | code[1];
        } break;

        case 0x3: {        // SKIP.EQI Vx,$NN
            if (cpu->V[code[0]&0xf] == code[1])
                cpu->PC+=2;
            cpu->PC+=2;
        } break;

        case 0x4: {        // SKIP.NEI Vx,$NN
            if (cpu->V[code[0]&0xf] != code[1])
                cpu->PC+=2;
            cpu->PC+=2;
        } break;

        case 0x5: {        // SKIP.EQ Vx,Vy
            if (cpu->V[code[0]&0xf] == cpu->V[code[1]])
                cpu->PC+=2;
            cpu->PC+=2;
        } break;

        case 0x6: {        // MVI Vx,$NN
            cpu->V[code[0]&0xf] = code[1];
            cpu->PC+=2;
        } break;

        case 0x7: {        // ADI Vx,$NN
            cpu->V[code[0]&0xf] += code[1];
            cpu->PC+=2;
        } break;

        case 0x8: {
            uint8_t lastnib = (code[1] >> 4);
            switch (lastnib) {
                case 0: {   // MOV Vx,Vy
                    cpu->V[code[0]&0xf] = cpu->V[lastnib];
                } break;
                case 1: {   // OR Vx,Vy
                    cpu->V[code[0]&0xf] |= cpu->V[lastnib];
                } break;
                case 2: {   // AND Vx,Vy
                    cpu->V[code[0]&0xf] &= cpu->V[lastnib];
                } break;
                case 3: {   // XOR Vx,Vy
                    cpu->V[code[0]&0xf] ^= cpu->V[lastnib];
                } break;
                case 4: {   // ADD Vx,Vy
                    cpu->V[code[0]&0xf] += cpu->V[lastnib];
                } break;
                case 5: {   // SUB Vx,Vy
                    cpu->V[code[0]&0xf] -= cpu->V[lastnib];
                } break;
                case 6: {   // SHR Vx,1
                    cpu->V[code[0]&0xf] >>= 1;
                } break;
                case 7: {   // SUB Vx,(Vy-Vx)
                    cpu->V[code[0]&0xf] &= cpu->V[lastnib];
                } break;
                case 0xe: { // SHL Vx,1
                    cpu->V[code[0]&0xf] <<= 1;
                } break;
            }
            cpu->PC+=2;
        } break;

        case 0x9: {       // SKIP.NE Vx,Vy
            if (cpu->V[code[0]&0xf] != cpu->V[code[1]])
                cpu->PC+=2;
            cpu->PC+=2;
        } break;

        case 0xa: {       // MVI I,$NNN
            cpu->I = code[0]&0xf | code[1];
            cpu->PC+=2;
        } break;

        case 0xb: {       // (JMP $NNN) + V0
            cpu->PC = cpu->V[0] + code[0]&0xf | code[1];
            cpu->PC+=2;
        } break;

        case 0xc: {       // RAND Vx,$NN
            cpu->V[code[0]&0xf] = (rand() % 256) & code[1];
            cpu->PC+=2;
        } break;

        case 0xd: Unimplemented(cpu); break;
        case 0xe: Unimplemented(cpu); break;
        case 0xf: Unimplemented(cpu); break;
    }

    return 0;
}