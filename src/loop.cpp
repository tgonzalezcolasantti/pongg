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
#include <ball.h>
#include <list.h>
#include <text.h>

TTF_Font* font;

ball_t ball;
list_adt obstacles;

void run_game_loop(SDL_Renderer* renderer){
    uint64_t ticks = 0;
    input_t input;
    init_input(&input);
    init_ball(&ball, load_texture(renderer, "./assets/img/player.png"));
    obstacles = create_list();
    collidable_t wall1 = {0, 0, 1920, 1};
    collidable_t wall2 = {0, 0, 1, 1080};
    collidable_t wall3 = {0, 1080, 1920, 1};
    collidable_t wall4 = {1920, 0, 1, 1080};

    append(obstacles, &wall1);
    append(obstacles, &wall2);
    append(obstacles, &wall3);
    append(obstacles, &wall4);

    font = TTF_OpenFont("./assets/fonts/arial.ttf", FONT_SIZE);

    while(1){
        handle_input(&input);
        prepareScene(renderer);
        run_frame(renderer, ticks, input);
        SDL_Texture* passion = getTextTexture("GRAPHIC DESIGN IS MY PASSION", font, renderer);
        blit(renderer, passion, 200, 200);
        presentScene(renderer);
        ticks = SDL_GetTicks64();
    }
}

void run_frame(SDL_Renderer* renderer, uint64_t lastTicks, input_t input){
    while (SDL_GetTicks64() - lastTicks < TICKS_FOR_NEXT_FRAME) {
        SDL_Delay(1);
    }
    move_ball(&ball, SDL_GetTicks64() - lastTicks, obstacles);
    draw_ball(renderer, &ball);
}   