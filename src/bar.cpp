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
#include <collider.h>

bar_t* create_bar(int x, int pspeed, const char* name, char* texture){
    bar_t* bar = (bar_t*)malloc(sizeof(bar_t));
    bar->x = x;
    bar->initialx = x;
    bar->y = BAR_INIT_Y;
    bar->w = BAR_WIDTH;
    bar->h = BAR_HEIGHT;
    bar->vx=0;
    bar->vy=0;
    bar->parry_vel = pspeed;
    bar->parry_time = 0;
    bar->texture = load_texture(texture);
    bar->name = name;
    bar_to_collider(bar, NULL);
    return bar;
}

void destroy_bar(bar_t* bar){
    SDL_DestroyTexture(bar->texture);
    free(bar);
}

void draw_bar(bar_t* bar){
    blit(bar->texture, {(int)bar->x, (int)bar->y, (int)bar->w, (int)bar->h}, 0);
}

collider_t* bar_to_collider(bar_t* bar, collider_t* collider){
    collider = set_object_collider(bar->x, bar->y, bar->vx, bar->vy, bar->w, bar->h, bar->name, VERY_HIGH_MASS, COLLIDER_RECT, COLLIDER_PRIORITY_LOW, bar, (collider_t*(*)(void*, collider_t*, double))move_bar, collider);
    return collider;
}

collider_t* move_bar(bar_t* bar, collider_t* collider, double dt){
    bar->y = bar->y + bar->vy * dt;
    if (bar->vx || bar->parry_time){
        bar->x = bar->x + bar->vx * dt;
        bar->parry_time += dt;
        if (bar->parry_time >= BAR_PARRY_TIME/2 && bar->parry_time <= BAR_PARRY_TIME){
            bar->vx = -bar->parry_vel;
        } else if (bar->parry_time >= BAR_PARRY_TIME){
            bar->vx = 0;
            bar->x = bar->initialx;
            bar->parry_time = 0;
        }
    }
    if (bar->y < 0) bar->y = 0;
    if (bar->y + bar->h > WINDOW_HEIGHT) bar->y = WINDOW_HEIGHT - bar->h;
    if (bar->x < 0) bar->x = 0;
    if (bar->x + bar->w > WINDOW_WIDTH) bar->x = WINDOW_WIDTH - bar->w;
    return bar_to_collider(bar, collider);
}

void set_bar_movement(bar_t* bar, char dir){
    if (dir > 0) bar->vy = -BAR_SPEED;
    else if (dir < 0) bar->vy = +BAR_SPEED;
    else bar->vy = 0;
}

void parry(bar_t* bar){
    if (!bar->parry_time){
        bar->x = bar->initialx;
        bar->vx = bar->parry_vel;
    }
}