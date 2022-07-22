#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>

#include "chip8.h"

#define PIXEL_SIZE 10

#define SCREEN_WIDTH (GFX_COLS * PIXEL_SIZE)
#define SCREEN_HEIGHT (GFX_ROWS * PIXEL_SIZE)

#define BLACK 0
#define WHITE 255

static int debug = 1;

Chip8* cpu;

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *screen;
SDL_Event event;
uint8_t *pixel_buffer;

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

void paint_pixel(int row, int col, uint8_t color) {
    row = SCREEN_HEIGHT - 1 - row;
    pixel_buffer[row * SCREEN_WIDTH + col] = color;
}

void paint_cell(int row, int col, uint8_t color) {
    int pixel_row = row * PIXEL_SIZE;
    int pixel_col = col * PIXEL_SIZE;
    int drow, dcol;

    for (drow = 0; drow < PIXEL_SIZE; drow++) {
        for (dcol = 0; dcol < PIXEL_SIZE; dcol++) {
            paint_pixel(pixel_row + drow, pixel_col + dcol, color);
        }
    }
}

void draw() {
    int position;

    for (int row = 0; row < GFX_ROWS; row++) {
        for (int col = 0; col < GFX_COLS; col++) {
            position = row * GFX_COLS + col;
            paint_cell(row, col, cpu->vram[position] ? WHITE : BLACK);
        }
    }
}

void main_loop(Chip8 *cpu) {
    while(SDL_PollEvent(&event) != 0) {
        switch (event.type) {
            case SDL_QUIT:
                cpu->halt = 1;
                break;
        }
    }

    emulate_op(cpu);
    
    if (cpu->draw_flag == 1) {
        draw();
        cpu->draw_flag = 0;
    }

    SDL_UpdateTexture(screen, NULL, pixel_buffer, sizeof(uint8_t) * SCREEN_WIDTH);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, screen, NULL, NULL);
    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {

    char filename[] = "rom/test_opcode.ch8";

    cpu = InitChip8();
    read_file_to_memory(cpu, filename, 0x200);

    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Chip-8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    screen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
    pixel_buffer = (Uint8*) malloc((SCREEN_WIDTH * SCREEN_HEIGHT) * sizeof(Uint8));

    printf("\nPC   B1 B2\n");
    printf("----------\n");
    while (cpu->halt == 0) {
        main_loop(cpu);
    }

    SDL_DestroyWindow(window);
    window = NULL;
    SDL_DestroyRenderer(renderer);
    renderer = NULL;
    SDL_Quit();

    return 0;
}