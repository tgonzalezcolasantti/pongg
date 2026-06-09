#ifndef __BAR_H
#define __BAR_H

#include <defs.h>
#include <SDL.h>
#include <ball.h>
#include <collider.h>

typedef struct bar_t{
    double initialx;
    double x;
    double y;
    double vx; 
    double vy;
    double w;
    double h;
    double parry_time;
    double parry_vel;
    SDL_Texture* texture;
    const char* name;
} bar_t;

#define P1 0
#define P2 1
#define P1_INIT_X 200
#define P2_INIT_X WINDOW_WIDTH - P1_INIT_X
#define BAR_INIT_Y WINDOW_HEIGHT/2
#define BAR_SPEED 1000
#define BAR_PARRY_TIME 0.2
#define BAR_PARRY_SPEED 1000
#define BAR_HEIGHT 400
#define BAR_WIDTH 20

bar_t* create_bar(int x, int pspeed, const char* name, char* texture);
void destroy_bar(bar_t* bar);
void draw_bar(bar_t* bar);
collider_t* bar_to_collider(bar_t* bar, collider_t* collider);
collider_t* move_bar(bar_t* bar, collider_t* collider, double dt);
void set_bar_movement(bar_t* bar, char dir);
void parry(bar_t* bar);

#endif