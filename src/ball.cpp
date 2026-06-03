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

void init_ball(ball_t* ball, SDL_Texture* texture){
    ball->x = 1000;
    ball->y = 500;
    ball->angle = 0;
    ball->vx= -BALL_SPEED_DEFAULT; //px/s
    ball->vy= BALL_SPEED_DEFAULT;  //px/s
    ball->newx=0;
    ball->newy=0;
    ball->radius=BALL_RADIUS;
    ball->texture = texture;
}

bool move_ball(ball_t* ball, long ticks, list_adt obstacles){
    ball->newx = ball->x + ball->vx * (ticks / 1000.0);
    ball->newy = ball->y + ball->vy * (ticks / 1000.0);

    if (check_collisions(ball, obstacles)){
        ball->newx = ball->x + ball->vx * (ticks / 1000.0);
        ball->newy = ball->y + ball->vy * (ticks / 1000.0);
    }
    ball->x = ball->newx;
    ball->y = ball->newy;
    int newangle = ball->angle + SDL_sqrt(ball->vx * ball->vx + ball->vy * ball->vy) * ticks / 5000.0;
    newangle = ball->angle + SDL_pow(newangle - ball->angle, 1.5);
    ball->angle = SDL_fmod(newangle, 360.0);
    dampen_speed(ball);
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Bpos %d %d", ball->x, ball->y);
    return (ball->x > 0) && (ball->x <= WINDOW_WIDTH) && (ball->y > 0) && (ball->y <= WINDOW_HEIGHT);
}

void dampen_speed(ball_t* ball){
    if (SDL_abs(ball->vx) > BALL_SPEED_DEFAULT) ball->vx *= BALL_SPEED_DAMPEN_FACTOR;
    if (SDL_abs(ball->vy) > BALL_SPEED_DEFAULT) ball->vy *= BALL_SPEED_DAMPEN_FACTOR;
}

bool check_collisions(ball_t* ball, list_adt obstacles){
    init_iterator(obstacles);
    bool hasBounced = false;

    while (has_next(obstacles)){
        collidable_t* collidable = (collidable_t*)next(obstacles);
        int cutoff = SDL_max(SDL_abs(ball->newx - ball->x), SDL_abs(ball->newy - ball->y));
        for (int i = 0; i <= cutoff; i++){  //Linearly interpolate movement and try to find collisions with more detail
            //collision while going up
            int newx = ball->x + ((double)(ball->newx - ball->x) / cutoff) * i;
            int newy = ball->y + ((double)(ball->newy - ball->y) / cutoff) * i;
            if (ball->y > collidable->lasty + collidable->h &&    // We were not colliding before
                newx + 2*ball->radius >= collidable->x && newx <= collidable->x + collidable->w && //We end up between range of object
                newy <= collidable->y + collidable->h){ //We traversed the thing
                    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION , "BOUNCE! while going up");
                    bounceoff(&ball->vy, &ball->vx, collidable->vx, collidable->vy);
                    hasBounced=true;
            }
    
            //collision while going down
            if (ball->y + 2*ball->radius < collidable->lasty &&    // We were not colliding before
                newx + 2*ball->radius >= collidable->x && newx <= collidable->x + collidable->w && //We end up between range of object
                newy + 2*ball->radius >= collidable->y){ //We traversed the thing
                    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION , "BOUNCE! while going down");
                    bounceoff(&ball->vy, &ball->vx, collidable->vx, collidable->vy);
                    hasBounced=true;
            }
    
            //collision while going left
            if (ball->x > collidable->lastx &&    // We were not colliding before
                newy + 2*ball->radius >= collidable->y && newy <= collidable->y + collidable->h && //We end up between range of object
                newx <= collidable->x){ //We traversed the thing
                    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION , "BOUNCE! while going left");
                    bounceoff(&ball->vx, &ball->vy, collidable->vy, collidable->vx);
                    hasBounced=true;
            }
    
            //collision while going right
            if (ball->x + 2*ball->radius < collidable->lastx + collidable->w &&    // We were not colliding before
                newy + 2*ball->radius >= collidable->y && newy <= collidable->y + collidable->h && //We end up between range of object
                newx + 2*ball->radius >= collidable->x + collidable->w){ //We traversed the thing
                    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION , "BOUNCE! while going right");
                    bounceoff(&ball->vx, &ball->vy, collidable->vy, collidable->vx);
                    hasBounced=true;
            }
            if (hasBounced) break; //No need to keep iterating, we know we collided
        }
    }
    return hasBounced;
}

void bounceoff(int* vnorm, int* vtan, int collider_vtan, int collider_vnorm){
    *vnorm = -*vnorm + collider_vnorm;
    if (SDL_abs(*vnorm) > abs(MAX_BALL_SPEED)) 
        *vnorm = (*vnorm >= 0) ? MAX_BALL_SPEED : -MAX_BALL_SPEED;
    *vtan = *vtan + collider_vtan / 2;
    if (SDL_abs(*vtan) > abs(MAX_BALL_SPEED)) 
        *vtan = (*vtan >= 0) ? MAX_BALL_SPEED : -MAX_BALL_SPEED;
}

void draw_ball(SDL_Renderer* renderer, ball_t* ball){
    SDL_Rect dest;

    dest.x = ball->x;
    dest.y = ball->y;
    dest.h = ball->radius*2;
    dest.w = ball->radius*2;
    
    blit(ball->texture, dest, ball->angle);
}