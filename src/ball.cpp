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

ball_t* create_ball(SDL_Texture* texture, char* name){
    ball_t* ball = (ball_t*)SDL_malloc(sizeof(ball_t));
    ball->x = 1000;
    ball->y = 500;
    ball->angle = 0;
    ball->vx= -BALL_SPEED_DEFAULT; //px/s
    ball->vy= BALL_SPEED_DEFAULT;  //px/s
    ball->radius=BALL_RADIUS;
    ball->texture=texture;
    ball->name=name;
    return ball;
}

void destroy_ball(ball_t* ball){
    SDL_free(ball);
}

collider_t* ball_to_collider(ball_t* ball){
    return create_collider(ball->x, ball->y, ball->vx, ball->vy, ball->radius*2, ball->radius*2, ball->name, DEFAULT_MASS, COLLIDER_CIRCLE, ball, (collider_t*(*)(void*, collider_t*, double))move_ball);
}

void dampen_speed(ball_t* ball){
    if (SDL_abs(ball->vx) > BALL_SPEED_DEFAULT) ball->vx *= BALL_SPEED_DAMPEN_FACTOR;
    if (SDL_abs(ball->vy) > BALL_SPEED_DEFAULT) ball->vy *= BALL_SPEED_DAMPEN_FACTOR;
    if (SDL_abs(ball->vx) < BALL_SPEED_DEFAULT/2) ball->vx *= 2;
}

collider_t* move_ball(ball_t* ball, collider_t* collider, double dt){
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Move ball");
    ball->x = collider->x;
    ball->y = collider->y;
    ball->vx = collider->vx;
    ball->vy = collider->vy;
    int newangle = ball->angle + SDL_sqrt(ball->vx * ball->vx + ball->vy * ball->vy) * dt / 5;
    newangle = ball->angle + SDL_pow(newangle - ball->angle, 1.5);
    ball->angle = SDL_fmod(newangle, 360.0);
    dampen_speed(ball);
    return ball_to_collider(ball);
}

void draw_ball(SDL_Renderer* renderer, ball_t* ball){
    SDL_Rect dest;

    dest.x = ball->x;
    dest.y = ball->y;
    dest.h = ball->radius*2;
    dest.w = ball->radius*2;
    
    blit(ball->texture, dest, ball->angle);
}