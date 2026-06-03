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
    input_t cmd = 0;
    if (!event->repeat){
        switch(event->keysym.scancode){
            case P1_UP_KEYCODE:
                cmd = P1_UP;
                break;
            case P1_DOWN_KEYCODE:
                cmd = P1_DOWN;
                break;
            case P1_LEFT_KEYCODE:
                cmd = P1_LEFT;
                break;
            case P1_RIGHT_KEYCODE:
                cmd = P1_RIGHT;
                break;
            case P2_UP_KEYCODE:
                cmd = P2_UP;
                break;
            case P2_DOWN_KEYCODE:
                cmd = P2_DOWN;
                break;
            case P2_LEFT_KEYCODE:
                cmd = P2_LEFT;
                break;
            case P2_RIGHT_KEYCODE:
                cmd = P2_RIGHT;
                break;
            case PAUSE_KEYCODE:
                cmd = PAUSE_CMD;
                break;
            default:
                break;
        }
    }
    if (cmd){
        if (event->type == SDL_KEYDOWN)
            ADD_COMMAND(input, cmd);
        else
            REMOVE_COMMAND(input, cmd);
    }
}