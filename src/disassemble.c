#include <stdint.h>
#include <stdio.h>

#include "disassemble.h"

void DisassembleCHIP8(uint8_t *codebuffer, int pc) {
    uint8_t *code = &codebuffer[pc];
    uint8_t firstnib = (code[0] >> 4);
    
    printf("%04x %02x %02x ", pc, code[0], code[1]);
    switch (firstnib) {    
        case 0x0: {
            switch (code[1]) {
                case 0xe0: printf("%-10s", "CLS"); break;    //clear screen
                case 0x00: printf("%-10s", "RTS"); break;    //return
                default: printf("Unknown 0."); break;
            }
        } break;
        case 0x1: printf("%-10s $%01x%02x", "JMP", code[0]&0xf, code[1]); break;
        case 0x2: printf("%-10s $%01x%02x", "CALL", code[0]&0xf, code[1]); break;
        case 0x3: printf("%-10s $%01x,$%02x", "SKIP.EQI", code[0]&0xf, code[1]); break;
        case 0x4: printf("%-10s $%01x,$%02x", "SKIP.NEI", code[0]&0xf, code[1]); break;
        case 0x5: printf("%-10s V%01x,V%02x", "SKIP.EQ", code[0]&0xf, code[1]); break;
        case 0x6: printf("%-10s V%01x,$%02x", "MVI", code[0]&0xf, code[1]); break;
        case 0x7: printf("%-10s V%01x,$%02x", "ADI", code[0]&0xf, code[1]); break;
        case 0x8: {
            uint8_t lastnib = (code[1] >> 4);
            switch (lastnib) {
                    case 0: printf("%-10s V%01x,V%02x", "MOV", code[0]&0xf, code[1]>>4); break;
                    case 1: printf("%-10s V%01x,V%02x", "OR", code[0]&0xf, code[1]>>4); break;
                    case 2: printf("%-10s V%01x,V%02x", "AND", code[0]&0xf, code[1]>>4); break;
                    case 3: printf("%-10s V%01x,V%02x", "XOR", code[0]&0xf, code[1]>>4); break;
                    case 4: printf("%-10s V%01x,V%02x", "ADD", code[0]&0xf, code[1]>>4); break;
                    case 5: printf("%-10s V%01X,V%01X,V%01X", "SUB", code[0]&0xf, code[0]&0xf, code[1]>>4); break;
                    case 6: printf("%-10s V%01X,V%01X", "SHR", code[0]&0xf, code[1]>>4); break;
                    case 7: printf("%-10s V%01X,V%01X,V%01X", "SUB.", code[0]&0xf, code[1]>>4, code[1]>>4); break;
                    case 0xe: printf("%-10s V%01X,V%01X", "SHL", code[0]&0xf, code[1]>>4); break;
                    default: printf("Unknown 8."); break;
                }
        } break;
        case 0x9: printf("%-10s I,$%01x%02x", "SKIP.NE", code[0]&0xf, code[1]>>4); break;
        case 0xa: printf("%-10s I,$%01x%02x", "MVI", code[0]&0xf, code[1]); break;
        case 0xb: printf("%-10s $%01x%02x + (V0)", "JMP", code[0]&0xf, code[1]); break;
        case 0xc: printf("%-10s V%01x,$%02x", "RAND", code[0]&0xf, code[1]); break;
        case 0xd: printf("%-10s V%01x,V%01x,$%02x", "SPRITE", code[0]&0xf, code[1]>>4, code[1]&0xf); break;
        case 0xe: {
            switch (code[1]) {
                case 0x9e: printf("%-10s V%01x", "KEY_Y"); break;
                case 0xa1: printf("%-10s V%01x", "KEY_N"); break;
            }
        } break;
        case 0xf: {
            switch (code[1]) {
                case 0x07: printf("%-10s V%01x,DELAY", "MOV", code[0]&0xf); break;
                case 0x0a: printf("%-10s V%01x", "KEY", code[0]&0xf); break;
                case 0x15: printf("%-10s DELAY,V%01x", "MOV", code[0]&0xf); break;
                case 0x18: printf("%-10s SOUND,V%01x", "MOV", code[0]&0xf); break;
                case 0x1e: printf("%-10s I,V%01x", "ADI", code[0]&0xf); break;
                case 0x29: printf("%-10s I,V%01x", "SPRITECHAR", code[0]&0xf); break;
                case 0x33: printf("%-10s I,V%01x", "MOVBCD", code[0]&0xf); break;
                case 0x55: printf("%-10s (I),V0-V%01x", "REG_DUMP", code[0]&0xf); break;
                case 0x65: printf("%-10s V0-V%01x,(I)", "REG_LOAD", code[0]&0xf); break;
            }
        } break;
    }    
}