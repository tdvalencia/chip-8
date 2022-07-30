#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <SDL.h>

#include "chip8.h"

#define PIXEL_SIZE 10

#define CLOCK_HZ 60

#define SCREEN_WIDTH (GFX_COLS * PIXEL_SIZE)
#define SCREEN_HEIGHT (GFX_ROWS * PIXEL_SIZE)

#define OFF 0x2d3047
#define ON  0x57b8ff

uint8_t keymap[16] = {
    SDLK_x,
    SDLK_1,
    SDLK_2,
    SDLK_3,
    SDLK_q,
    SDLK_w,
    SDLK_e,
    SDLK_a,
    SDLK_s,
    SDLK_d,
    SDLK_z,
    SDLK_c,
    SDLK_4,
    SDLK_r,
    SDLK_f,
    SDLK_v,
};

Chip8* cpu;

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *screen;
uint32_t *pixel_buffer;

int sleep;
uint32_t next_game_tick;
SDL_Event event;

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

void draw() {
    for (int i = 0; i < GFX_SIZE; i++) {
        uint8_t pixel = cpu->vram[i];
        pixel_buffer[i] = pixel ? ON : OFF;
    }

    SDL_UpdateTexture(screen, NULL, pixel_buffer, GFX_COLS * sizeof(Uint32));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, screen, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void main_loop(Chip8 *cpu) {
    emulate_op(cpu);

    while(SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) cpu->halt = 1;

        if (event.type == SDL_KEYDOWN) {        
            for (int i = 0; i < 16; ++i) {
                if (event.key.keysym.sym == keymap[i]) {
                    cpu->key_state[i] = 1;
                }
            }
        }

        if (event.type == SDL_KEYUP) {
            for (int i = 0; i < 16; i++) {
                if (event.key.keysym.sym == keymap[i]) {
                    cpu->key_state[i] = 0;
                }
            }
        }
    }
    
    if (cpu->draw_flag == 1) {
        draw();
        cpu->draw_flag = 0;
    }

    next_game_tick += CLOCK_HZ / 10;
    sleep = next_game_tick - SDL_GetTicks();

    if( sleep >= 0 ) {
        SDL_Delay(sleep);
    }
}

int main(int argc, char* argv[]) {
    cpu = InitChip8();
    if (argc == 2) {
        read_file_to_memory(cpu, argv[1], 0x200);
    }
    else {
        printf("Invalid number of arguments. Use \"./chip8.exe {name of rom}\".\n");
        goto close_game;
    }

    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Chip-8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    screen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, GFX_COLS, GFX_ROWS);

    sleep = 0;
    next_game_tick = SDL_GetTicks();
    pixel_buffer = (Uint32*) malloc((SCREEN_WIDTH * SCREEN_HEIGHT) * sizeof(Uint32));

    printf("\nPC   B1 B2\n");
    printf("----------\n");
    while (cpu->halt == 0) {
        main_loop(cpu);

    }

    close_game:
    SDL_DestroyWindow(window);
    window = NULL;
    SDL_DestroyRenderer(renderer);
    renderer = NULL;
    SDL_Quit();

    return 0;
}
