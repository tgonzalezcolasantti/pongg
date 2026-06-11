// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
using namespace std;

#include <iostream>
#include <SDL.h>
#include <input.h>

void init_input(input_t* input){
    SDL_memset(input, 0, sizeof(input_t));
}

void handle_input(input_t* input){
    SDL_Event event;

    while (SDL_PollEvent(&event)){
        switch (event.type){
            case SDL_QUIT:
                exit(0);
                break;
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                handle_key(&event.key, input);
                break;
            default:
                break;
        }
    }
}

void handle_key(SDL_KeyboardEvent *event, input_t* input){
    bool isPressed = event->type == SDL_KEYDOWN;
    if (!event->repeat){
        switch(event->keysym.scancode){
            case P1_UP_KEYCODE:
                input->p1_up = isPressed;
                break;
            case P1_DOWN_KEYCODE:
                input->p1_down = isPressed;
                break;
            case P1_RIGHT_KEYCODE:
                input->p1_parry = isPressed;
                break;
            case P2_UP_KEYCODE:
                input->p2_up = isPressed;
                break;
            case P2_DOWN_KEYCODE:
                input->p2_down = isPressed;
                break;
            case P2_LEFT_KEYCODE:
                input->p2_parry = isPressed;
                break;
            case PAUSE_KEYCODE:
                input->p1_up = isPressed;
                break;
            default:
                break;
        }
    }
}