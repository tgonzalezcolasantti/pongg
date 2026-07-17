// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
using namespace std;

#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#include <defs.h>
#include <draw.h>
#include <text.h>
#include <input.h>
#include <character_selector.h>
#include <vector>
#include <character.h>
#include <parser.h>
#include <character_show.h>

SDL_Window* window;
SDL_Renderer* renderer;
input_t input;
SDL_Texture* selector_bg;
SDL_Texture* character_bg;
TTF_Font* font;

int main(int argc, char* argv[])
{    
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0){
        const char* message = SDL_GetError();
        cerr << "Error initializing SDL: " << (message ? message : "Unknown error");
    }
    
    if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) == 0){
        cerr << "Couldn't initialize SDL Image: " << IMG_GetError();
        exit(1);
    }

    if (TTF_Init() < 0){
        cerr << "Couldn't initialize SDL TTF: " << TTF_GetError();
        exit(1);
    }

    Mix_Init(MIX_INIT_MP3);
    if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 512) != 0){
        cout << "Couldn't initialize SDL Mixer: " << Mix_GetError() << "... Damn\n";
        //exit(1);
    }
    Mix_AllocateChannels(8);
    Mix_Music* bg = Mix_LoadMUS("./assets/bgm/esdiel.mp3");
    Mix_PlayMusic(bg, -1);

    window = SDL_CreateWindow(PROGRAM_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
    if (!window){
        const char* message = SDL_GetError();
        cerr << "Error creating window: " << (message ? message : "Unknown error");
    }
    
    //SDL_Surface* windowIcon = IMG_Load(ASSET_BALL);
    //SDL_SetWindowIcon(window, windowIcon);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_SOFTWARE);
    if (!renderer){
        const char* message = SDL_GetError();
        cerr << "Error initializing renderer: " << (message ? message : "Unknown error");
    }
    
    init_input(&input);
    font = TTF_OpenFont(ASSET_FONT, FONT_SIZE);

    selector_bg = IMG_LoadTexture(renderer, ASSET_BG);
    character_bg = IMG_LoadTexture(renderer, ASSET_BG);
    
    //SDL_Texture* passion = getTextTexture(font, "GRAPHIC DESIGN IS MY PASSION");
    //blit(passion, {100, 400, 1800, 300}, -25);

    vector<character_t*> characters;
    cout << parse(characters) << endl;
    dump(characters);
    while(true){
        ssize_t selection = select_character(characters);
        if(selection>=0){
            show_character(characters[selection]);
        } else{
            break;
        }
    }

    SDL_DestroyTexture(selector_bg);
    SDL_DestroyTexture(character_bg);
    return 0;
}








