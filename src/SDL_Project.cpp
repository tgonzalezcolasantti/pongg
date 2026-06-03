
using namespace std;

#include <iostream>
#include "SDL_image.h"
#include <SDL_ttf.h>

SDL_Event event;


int main(int argc, char* argv[])
{    
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window = SDL_CreateWindow("Prueba", 100, 100, 1280, 768, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    SDL_SetRenderDrawColor(renderer, 0, 0, 40, 0);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    
    SDL_Delay(5000);
 
    return 0;
}








