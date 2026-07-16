// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
using namespace std;

#include <iostream>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <defs.h>
#include <loop.h>
#include <input.h>
#include <draw.h>
#include <text.h>

extern SDL_Renderer* renderer;

SDL_Texture* bg;

void run_game_loop(){
    uint64_t ticks = 0;
    input_t input;
    init_input(&input);
    init_text();

    bg = load_texture(ASSET_BG);

    while(1){
        handle_input(&input);
        prepareScene(bg);
        SDL_Texture* passion = getTextTexture("GRAPHIC DESIGN IS MY PASSION");
        easyblit(passion, 200, 200);
        presentScene();
        ticks = SDL_GetTicks64();
    }
}
