#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>

#include "chip8.h"

static int debug = 1;
static Chip8* cpu;

static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Texture *screen;
static SDL_Event event;

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

void main_loop(Chip8 *cpu) {
    while(SDL_PollEvent(&event) != 0) {
        switch (event.type) {
            case SDL_QUIT:
                cpu->halt = 1;
                break;
        }
    }

    emulate_op(cpu);

    SDL_UpdateTexture(screen, NULL, cpu->screen, SCREEN_WIDTH * sizeof(uint8_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, screen, NULL, NULL);
    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {

    char filename[] = "rom/ibm.ch8";

    cpu = InitChip8();
    read_file_to_memory(cpu, filename, 0x200);

    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Chip-8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH * 8, SCREEN_HEIGHT * 8, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    screen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

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