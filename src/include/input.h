#ifndef __INPUT_H
#define __INPUT_H

#include <stdbool.h>
#include <stdint.h>
#include <SDL_keycode.h>

#define P1_UP_KEYCODE       SDL_SCANCODE_W
#define P1_DOWN_KEYCODE     SDL_SCANCODE_S
#define P1_LEFT_KEYCODE     SDL_SCANCODE_A
#define P1_RIGHT_KEYCODE    SDL_SCANCODE_D
#define P2_UP_KEYCODE       SDL_SCANCODE_UP
#define P2_DOWN_KEYCODE     SDL_SCANCODE_DOWN
#define P2_LEFT_KEYCODE     SDL_SCANCODE_LEFT
#define P2_RIGHT_KEYCODE    SDL_SCANCODE_RIGHT

#define PAUSE_KEYCODE       SDL_SCANCODE_ESCAPE

typedef struct input_t {
    bool p1_up;
    bool p1_down;
    bool p1_parry;
    bool p2_up;
    bool p2_down;
    bool p2_parry;
    bool pause;
} input_t;

void handle_key(SDL_KeyboardEvent *event, input_t* input);
void handle_input(input_t* input);
void init_input(input_t* input);

#endif