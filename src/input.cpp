using namespace std;

#include <iostream>
#include <SDL.h>
#include <input.h>

void init_input(input_t* input){
    SDL_memset(input, 0, sizeof(input_t));
}

void handle_input(input_t* input, bool clear_last_input){
    SDL_Event event;
    if (clear_last_input)
        init_input(input);
    
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
    switch(event->keysym.scancode){
        case UP_KEYCODE:
            input->up = isPressed;
            break;
        case DOWN_KEYCODE:
            input->down = isPressed;
            break;
        case BACK_KEYCODE:
            input->back = isPressed;
            break;
        case SELECT_KEYCODE:
            input->select = isPressed;
            break;
        default:
            break;
    }

}