#ifndef __INPUT_H
#define __INPUT_H

#include <stdbool.h>
#include <stdint.h>
#include <SDL_keycode.h>

#define UP_KEYCODE          SDL_SCANCODE_UP
#define DOWN_KEYCODE        SDL_SCANCODE_DOWN
#define SELECT_KEYCODE      SDL_SCANCODE_KP_ENTER
#define BACK_KEYCODE        SDL_SCANCODE_ESCAPE

typedef struct input_t {
    bool up;
    bool down;
    bool back;
    bool select;
} input_t;

void handle_key(SDL_KeyboardEvent *event, input_t* input);
void handle_input(input_t* input, bool clear_last_input);
void init_input(input_t* input);

#endif