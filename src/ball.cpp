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
#include <collider.h>

ball_t* create_ball(char* name, char* texture){
    ball_t* ball = (ball_t*)malloc(sizeof(ball_t));
    ball->x = 1000;
    ball->y = 500;
    ball->angle = 0;
    ball->vx= -BALL_SPEED_DEFAULT; //px/s
    ball->vy= BALL_SPEED_DEFAULT;  //px/s
    ball->radius=BALL_RADIUS;
    ball->texture=load_texture(texture);
    ball->name=name;
    ball_to_collider(ball, NULL);
    return ball;
}

void destroy_ball(ball_t* ball){
    SDL_DestroyTexture(ball->texture);
    free(ball);
}

collider_t* ball_to_collider(ball_t* ball, collider_t* collider){
    return set_object_collider(ball->x, ball->y, ball->vx, ball->vy, ball->radius*2, ball->radius*2, ball->name, DEFAULT_MASS, COLLIDER_CIRCLE, COLLIDER_PRIORITY_NORMAL, ball, (collider_t*(*)(void*, collider_t*, double))move_ball, collider);
}

void dampen_speed(ball_t* ball, double dt){
    if (abs(ball->vx) > BALL_SPEED_DEFAULT) ball->vx *= pow(BALL_SPEED_DAMPEN_FACTOR, dt);
    if (abs(ball->vy) > BALL_SPEED_DEFAULT) ball->vy *= pow(BALL_SPEED_DAMPEN_FACTOR, dt);
    if (abs(ball->vx) < BALL_SPEED_DEFAULT/2) ball->vx *= 2;
}

collider_t* move_ball(ball_t* ball, collider_t* collider, double dt){
    ball->x = collider->x;
    ball->y = collider->y;
    ball->vx = SDL_max(-MAX_BALL_SPEED, SDL_min(collider->vx, MAX_BALL_SPEED));
    ball->vy = SDL_max(-MAX_BALL_SPEED, SDL_min(collider->vy, MAX_BALL_SPEED));
    int newangle = ball->angle + sqrt(ball->vx * ball->vx + ball->vy * ball->vy) * dt / 5;
    newangle = ball->angle + pow(newangle - ball->angle, 1.5);
    ball->angle = fmod(newangle, 360.0);
    dampen_speed(ball, dt);
    return ball_to_collider(ball, collider);
}

void draw_ball(SDL_Renderer* renderer, ball_t* ball){
    SDL_Rect dest;

    dest.x = (int)ball->x;
    dest.y = (int)ball->y;
    dest.h = (int)ball->radius*2;
    dest.w = (int)ball->radius*2;
    
    blit(ball->texture, dest, ball->angle);
}

bool is_inside(ball_t* ball){
    int ballx = ball->x + ball->radius;
    int bally = ball->y + ball->radius;
    return ballx >= 0 && ballx < WINDOW_WIDTH && bally >= 0 && bally < WINDOW_HEIGHT;
}