#ifndef __BALL_H
#define __BALL_H

#include <SDL.h>
#include <list.h>
#include <collider.h>

#define MAX_BALL_SPEED 4000
#define BALL_SPEED_DEFAULT 500
#define BALL_RADIUS 100
#define BALL_SPEED_DAMPEN_FACTOR 0.995

typedef struct ball_t{
    int x;
    int y;
    int vx;
    int vy;
    int radius;
    SDL_Texture* texture;
    const char* name;
    int angle;
} ball_t;

collider_t* move_ball(ball_t* ball, collider_t* collider, double dt);
ball_t* create_ball(SDL_Texture* texture, char* name);
void draw_ball(SDL_Renderer* renderer, ball_t* ball);
collider_t* ball_to_collider(ball_t* ball);

#endif