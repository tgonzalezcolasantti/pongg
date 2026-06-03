#ifndef __BALL_H
#define __BALL_H

#include <SDL.h>
#include <list.h>

#define MAX_BALL_SPEED 4000
#define BALL_SPEED_DEFAULT 500
#define BALL_RADIUS 100
#define BALL_SPEED_DAMPEN_FACTOR 0.995
#define COLLIDER_PRECISION 100

typedef struct ball_t{
    int x;
    int y;
    int newx;
    int newy;
    int vx;
    int vy;
    int radius;
    SDL_Texture* texture;
    int angle;
} ball_t;

typedef struct collidable_t{
    int x;
    int y;
    int lastx;
    int lasty;
    int w;
    int h;
    int vx;
    int vy;
} collidable_t;

bool check_collisions(ball_t* ball, list_adt obstacles);
bool move_ball(ball_t* ball, long ticks, list_adt obstacles);
void init_ball(ball_t* ball, SDL_Texture* texture);
void draw_ball(SDL_Renderer* renderer, ball_t* ball);
void bounceoff(int* vnorm, int* vtan, int collider_vtan, int collider_vnorm);
void dampen_speed(ball_t* ball);

#endif