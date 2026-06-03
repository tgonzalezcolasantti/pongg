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
    ball->vx= -2000; //px/s
    ball->vy= 2000;  //px/s
    ball->newx=0;
    ball->newy=0;
    ball->radius=100;
    ball->texture = texture;
}

void move_ball(ball_t* ball, long ticks, list_adt obstacles){
    ball->newx = ball->x + ball->vx * (ticks / 1000.0);
    ball->newy = ball->y + ball->vy * (ticks / 1000.0);

    if (check_collisions(ball, obstacles)){
        ball->newx = ball->x + ball->vx * (ticks / 1000.0);
        ball->newy = ball->y + ball->vy * (ticks / 1000.0);
    }
    ball->x = ball->newx;
    ball->y = ball->newy;
}

bool check_collisions(ball_t* ball, list_adt obstacles){
    init_iterator(obstacles);
    bool hasBounced = false;

    while (has_next(obstacles)){
        collidable_t* collidable = (collidable_t*)next(obstacles);

        //collision while going up
        if (ball->y > collidable->y + collidable->h &&    // We were not colliding before
            ball->newx + 2*ball->radius >= collidable->x && ball->newx <= collidable->x + collidable->w && //We end up between range of object
            ball->newy <= collidable->y + collidable->h){ //We traversed the thing
                SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION , "BOUNCE! while going up");
                bounceoff(&ball->vy, &ball->vx, collidable->vx);
                hasBounced=true;
        }

        //collision while going down
        if (ball->y + 2*ball->radius < collidable->y &&    // We were not colliding before
            ball->newx + 2*ball->radius >= collidable->x && ball->newx <= collidable->x + collidable->w && //We end up between range of object
            ball->newy + 2*ball->radius >= collidable->y){ //We traversed the thing
                SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION , "BOUNCE! while going down");
                bounceoff(&ball->vy, &ball->vx, collidable->vx);
                hasBounced=true;
        }

        //collision while going left
        if (ball->x > collidable->x &&    // We were not colliding before
            ball->newy + 2*ball->radius >= collidable->y && ball->newy <= collidable->y + collidable->h && //We end up between range of object
            ball->newx <= collidable->x){ //We traversed the thing
                SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION , "BOUNCE! while going left");
                bounceoff(&ball->vx, &ball->vy, collidable->vy);
                hasBounced=true;
        }

        //collision while going right
        if (ball->x + 2*ball->radius < collidable->x + collidable->w &&    // We were not colliding before
            ball->newy + 2*ball->radius >= collidable->y && ball->newy <= collidable->y + collidable->h && //We end up between range of object
            ball->newx + 2*ball->radius >= collidable->x + collidable->w){ //We traversed the thing
                SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION , "BOUNCE! while going right");
                bounceoff(&ball->vx, &ball->vy, collidable->vy);
                hasBounced=true;
        }
    }
    return hasBounced;
}

void bounceoff(int* vnorm, int* vtan, int collider_vtan){
    *vnorm = -*vnorm;
    *vtan = *vtan + collider_vtan;
}

void draw_ball(SDL_Renderer* renderer, ball_t* ball){
    SDL_Rect dest;

    dest.x = ball->x;
    dest.y = ball->y;
    dest.h = ball->radius*2;
    dest.w = ball->radius*2;
    
    SDL_RenderCopy(renderer, ball->texture, NULL, &dest);
}