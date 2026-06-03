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
#include <bar.h>

void init_bar(bar_t* bar, SDL_Texture* texture, int x, int pspeed){
    SDL_memset(bar, 0, sizeof(bar_t));
    bar->x = x;
    bar->initialx = x;
    bar->y = BAR_INIT_Y;
    bar->w = BAR_WIDTH;
    bar->h = BAR_HEIGHT;
    bar->parry_vel = pspeed;
    bar->texture = texture;
}

void draw_bar(bar_t* bar){
    blit(bar->texture, {bar->x, bar->y, bar->w, bar->h}, 0);
}

collidable_t* bar_to_collider(bar_t* bar){
    bar->collider.lastx = bar->collider.x;
    bar->collider.lasty = bar->collider.y;
    bar->collider.x = bar->x;
    bar->collider.y = bar->y;
    bar->collider.vx = bar->vx;
    bar->collider.vy = bar->vy;
    bar->collider.w = bar->w;
    bar->collider.h = bar->h;
    return &bar->collider;
}

void move_bar(bar_t* bar, long ticks){
    bar->y = bar->y + bar->vy * (ticks / 1000.0);
    if (bar->vx || bar->parry_step){
        bar->x = bar->x + bar->vx * (ticks / 1000.0);
        bar->parry_step++;
        if (bar->parry_step == BAR_PARRY_STEPS/2){
            bar->vx = -bar->vx;
        } else if (bar->parry_step >= BAR_PARRY_STEPS){
            bar->vx = 0;
            bar->x = bar->initialx;
            bar->parry_step = 0;
        }
    }
    if (bar->y < 0) bar->y = 0;
    if (bar->y + bar->h > WINDOW_HEIGHT) bar->y = WINDOW_HEIGHT - bar->h;
    if (bar->x < 0) bar->x = 0;
    if (bar->x + bar->w > WINDOW_WIDTH) bar->x = WINDOW_WIDTH - bar->w;
    bar_to_collider(bar);
}

void set_bar_movement(bar_t* bar, char dir){
    if (dir > 0) bar->vy = -BAR_SPEED;
    else if (dir < 0) bar->vy = +BAR_SPEED;
    else bar->vy = 0;
}

void parry(bar_t* bar){
    if (!bar->parry_step){
        bar->x = bar->initialx;
        bar->vx = bar->parry_vel;
    }
}