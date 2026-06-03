#ifndef __INPUT_H
#define __INPUT_H

#include <stdbool.h>
#include <stdint.h>
#include <SDL_keycode.h>

#define P1_UP       1
#define P1_DOWN     1 << 1
#define P1_LEFT     1 << 2
#define P1_RIGHT    1 << 3
#define P2_UP       1 << 4
#define P2_DOWN     1 << 5
#define P2_LEFT     1 << 6
#define P2_RIGHT    1 << 7
#define PAUSE_CMD   1 << 8

#define P1_UP_KEYCODE       SDL_SCANCODE_W
#define P1_DOWN_KEYCODE     SDL_SCANCODE_S
#define P1_LEFT_KEYCODE     SDL_SCANCODE_A
#define P1_RIGHT_KEYCODE    SDL_SCANCODE_D
#define P2_UP_KEYCODE       SDL_SCANCODE_UP
#define P2_DOWN_KEYCODE     SDL_SCANCODE_DOWN
#define P2_LEFT_KEYCODE     SDL_SCANCODE_LEFT
#define P2_RIGHT_KEYCODE    SDL_SCANCODE_RIGHT

#define PAUSE_KEYCODE       SDL_SCANCODE_ESCAPE

#define ADD_COMMAND(input, cmd)  *input |= (cmd)
#define REMOVE_COMMAND(input, cmd)  *input &= UINT32_MAX - (cmd)
#define HAS_COMMAND(input, cmd) (input) & (cmd)

typedef uint32_t input_t;

void handle_key(SDL_KeyboardEvent *event, input_t* input);
void handle_input(input_t* input);
void init_input(input_t* input);

#endif