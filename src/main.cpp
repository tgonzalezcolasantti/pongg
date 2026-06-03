// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
using namespace std;

#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#include <defs.h>
#include <loop.h>
#include <draw.h>

SDL_Window* window;
SDL_Renderer* renderer;

int main(int argc, char* argv[])
{    
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0){
        const char* message = SDL_GetError();
        cerr << "Error initializing SDL: " << (message ? message : "Unknown error");
    }
    
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

    if (TTF_Init() < 0){
        cerr << "Couldn't initialize SDL TTF: " << SDL_GetError();
        exit(1);
    }

    window = SDL_CreateWindow(GAME_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window){
        const char* message = SDL_GetError();
        cerr << "Error creating window: " << (message ? message : "Unknown error");
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_SOFTWARE);
    if (!renderer){
        const char* message = SDL_GetError();
        cerr << "Error initializing renderer: " << (message ? message : "Unknown error");
    }
    
    run_game_loop();
     
    return 0;
}








