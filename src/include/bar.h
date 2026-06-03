#ifndef __BAR_H
#define __BAR_H

#include <defs.h>
#include <SDL.h>
#include <ball.h>

typedef struct bar_t{
    int initialx;
    int x;
    int y;
    int vx; 
    int vy;
    int w;
    int h;
    int parry_step;
    int parry_vel;
    SDL_Texture* texture;
    collidable_t collider;
} bar_t;

#define P1 0
#define P2 1
#define P1_INIT_X 200
#define P2_INIT_X WINDOW_WIDTH - P1_INIT_X
#define BAR_INIT_Y WINDOW_HEIGHT/2
#define BAR_SPEED 1000
#define BAR_PARRY_STEPS 10
#define BAR_PARRY_SPEED 1000
#define BAR_HEIGHT 400
#define BAR_WIDTH 20

void init_bar(bar_t* bar, SDL_Texture* texture, int x, int pspeed);
void draw_bar(bar_t* bar);
collidable_t* bar_to_collider(bar_t* bar);
void move_bar(bar_t* bar, long ticks);
void set_bar_movement(bar_t* bar, char dir);
void parry(bar_t* bar);

#endif