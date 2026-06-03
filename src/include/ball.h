#ifndef __BALL_H
#define __BALL_H

#include <SDL.h>
#include <list.h>

typedef struct ball_t{
    int x;
    int y;
    int newx;
    int newy;
    int vx;
    int vy;
    int radius;
    SDL_Texture* texture;
} ball_t;

typedef struct collidable_t{
    int x;
    int y;
    int w;
    int h;
    int vx;
    int vy;
} collidable_t;

bool check_collisions(ball_t* ball, list_adt obstacles);
void move_ball(ball_t* ball, long ticks, list_adt obstacles);
void init_ball(ball_t* ball, SDL_Texture* texture);
void draw_ball(SDL_Renderer* renderer, ball_t* ball);
void bounceoff(int* vnorm, int* vtan, int collider_vtan);

#endif