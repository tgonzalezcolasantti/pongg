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
#include <bar.h>

extern SDL_Renderer* renderer;

SDL_Texture* bg;
ball_t* ball;
bar_t* bars[2] = {0};
bool pause = false;

collider_t* wall_collider_top(void* useless, collider_t* coll, double useless3){
    return create_collider(0, 0, 0, 0, WINDOW_WIDTH, 10, "walltop", VERY_HIGH_MASS, COLLIDER_RECT, COLLIDER_PRIORITY_HIGH, (void*)"walltop", wall_collider_top, coll);
}
collider_t* wall_collider_bottom(void* useless, collider_t* coll, double useless3){
    return create_collider(0, WINDOW_HEIGHT, 0, 0, WINDOW_WIDTH, 10, "wallbottom", VERY_HIGH_MASS, COLLIDER_RECT, COLLIDER_PRIORITY_HIGH, (void*)"wallbottom", wall_collider_bottom, coll);
}
collider_t* wall_collider_left(void* useless, collider_t* coll, double useless3){
    return create_collider(0, 0, 0, 0, 10, WINDOW_HEIGHT, "wallleft", VERY_HIGH_MASS, COLLIDER_RECT, COLLIDER_PRIORITY_HIGH, (void*)"wallleft", wall_collider_left, coll);
}
collider_t* wall_collider_right(void* useless, collider_t* coll, double useless3){
    return create_collider(WINDOW_WIDTH, 0, 0, 0, 10, WINDOW_HEIGHT, "wallright", VERY_HIGH_MASS, COLLIDER_RECT, COLLIDER_PRIORITY_HIGH, (void*)"wallright", wall_collider_right, coll);
}

void run_game_loop(){
    uint64_t ticks = 0;
    input_t input;
    init_input(&input);
    init_text();
    init_collider();
    bg = load_texture(ASSET_BG);

    wall_collider_top(NULL, NULL, 0);
    wall_collider_bottom(NULL, NULL, 0);
    // wall_collider_left(NULL, NULL, 0);
    // wall_collider_right(NULL, NULL, 0);
    ball = create_ball(load_texture(ASSET_BALL), "ball");
    bars[P1] = create_bar(load_texture(ASSET_BAR), P1_INIT_X, BAR_PARRY_SPEED, "P1");
    bars[P2] = create_bar(load_texture(ASSET_BAR), P2_INIT_X, -BAR_PARRY_SPEED, "P2");

    while(1){
        handle_input(&input);
        prepareScene(bg);
        if (!run_frame(ticks, input)) return;
        //SDL_Texture* passion = getTextTexture("GRAPHIC DESIGN IS MY PASSION", font);
        //easyblit(passion, 200, 200);
        presentScene();
        ticks = SDL_GetTicks64();
    }
}

bool run_frame(uint64_t lastTicks, input_t input){
    while (SDL_GetTicks64() - lastTicks < TICKS_FOR_NEXT_FRAME) {
        SDL_Delay(1);
    }
    if (HAS_COMMAND(input, P1_UP)){
        set_bar_movement(bars[P1], 1);
    } else if (HAS_COMMAND(input, P1_DOWN)){
        set_bar_movement(bars[P1], -1);
    } else {
        set_bar_movement(bars[P1], 0);
    }
    if (HAS_COMMAND(input, P2_UP)){
        set_bar_movement(bars[P2], 1);
    } else if (HAS_COMMAND(input, P2_DOWN)){
        set_bar_movement(bars[P2], -1);
    } else {
        set_bar_movement(bars[P2], 0);
    }

    if(HAS_COMMAND(input, P1_RIGHT)){
        parry(bars[P1]);
    }

    if(HAS_COMMAND(input, P2_LEFT)){
        parry(bars[P2]);
    }
    if (HAS_COMMAND(input, PAUSE_CMD)){
        pause = !pause;
    }
    if (!pause) update_collider((SDL_GetTicks64() - lastTicks) / 1000.0);

    draw_bar(bars[P1]);
    draw_bar(bars[P2]);

    draw_ball(renderer, ball);
    return true;
}   