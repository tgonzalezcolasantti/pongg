#ifndef __BALL_H
#define __BALL_H

#include <SDL.h>
#include <list.h>

#define MAX_BALL_SPEED 4000
#define BALL_SPEED_DEFAULT 500
#define BALL_RADIUS 100
#define BALL_SPEED_DAMPEN_FACTOR 0.995
#define COLLIDER_PRECISION 100
#define MAX_COLLIDER_ATTEMPTS 10
#define COLLIDER_GRACE_PERIOD 2

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
    bool locked;
    int gracePeriod;
} ball_t;

bool check_collisions(ball_t* ball, list_adt obstacles);
bool move_ball(ball_t* ball, long ticks, list_adt obstacles);
void init_ball(ball_t* ball, SDL_Texture* texture);
void draw_ball(SDL_Renderer* renderer, ball_t* ball);
void bounceoff(int* vnorm, int* vtan, int collider_vtan, int collider_vnorm, double angle, bool shouldFlipAgain);
void dampen_speed(ball_t* ball);
bool check_line_cross(int a1, int b1, int a2, int b2, int c1, int d1, int c2, int d2, int lenm, int lenfixed);
double check_circle_cross_line(int ca1, int cb1, int ca2, int cb2, int la1, int lb1, int la2, int lb2, int rad, int ll);

#endif