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
    ball->locked = false;
    ball->gracePeriod = 0;
}

bool move_ball(ball_t* ball, long ticks, list_adt obstacles){
    int oldx = ball->x;
    int oldy = ball->y;
    if (!ball->locked || ball->gracePeriod){
        ball->newx = ball->x + ball->vx * (ticks / 1000.0);
        ball->newy = ball->y + ball->vy * (ticks / 1000.0);
    } else {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Ball is locked :(");
    }

    bool collided = false;
    int attempts = 0;
    if (!ball->gracePeriod){
        do{
            //collided = check_collisions(ball, obstacles);
            if (collided){
                ball->newx = ball->x + ball->vx * (ticks / 1000.0);
                ball->newy = ball->y + ball->vy * (ticks / 1000.0);
                ball->gracePeriod = COLLIDER_GRACE_PERIOD;
            }
            ball->x = ball->newx;
            ball->y = ball->newy;
            attempts++;
        } while(collided && attempts<MAX_COLLIDER_ATTEMPTS);    //We set a limit so that if we trap a ball it wont lock the whole game
        if (attempts >= MAX_COLLIDER_ATTEMPTS){
            ball->locked=true;
            ball->x = oldx;
            ball->y = oldy;
        } else {
            ball->locked=false;
        }
    } else {
        ball->gracePeriod--;
    }
    int newangle = ball->angle + SDL_sqrt(ball->vx * ball->vx + ball->vy * ball->vy) * ticks / 5000.0;
    newangle = ball->angle + SDL_pow(newangle - ball->angle, 1.5);
    ball->angle = SDL_fmod(newangle, 360.0);
    dampen_speed(ball);
    return (ball->x > 0) && (ball->x <= WINDOW_WIDTH) && (ball->y > 0) && (ball->y <= WINDOW_HEIGHT);
}

void dampen_speed(ball_t* ball){
    if (SDL_abs(ball->vx) > BALL_SPEED_DEFAULT) ball->vx *= BALL_SPEED_DAMPEN_FACTOR;
    if (SDL_abs(ball->vy) > BALL_SPEED_DEFAULT) ball->vy *= BALL_SPEED_DAMPEN_FACTOR;
    if (SDL_abs(ball->vx) < BALL_SPEED_DEFAULT/2) ball->vx *= 2;
}

// bool check_collisions(ball_t* ball, list_adt obstacles){
//     init_iterator(obstacles);
//     bool hasBounced = false;

//     while (has_next(obstacles)){
//         collidable_t* obs = (collidable_t*)next(obstacles);
//         double angle = NAN;
//         if (ball->y >= obs->lasty){
//             //collision while going up
//             //if (check_line_cross(ball->y, ball->x, ball->newy, ball->newx, obs->lasty, obs->lastx + obs->h, obs->y, obs->x + obs->h, 2*ball->radius, obs->w)){
//             angle = check_circle_cross_line(ball->y, ball->x, ball->newy, ball->newx, obs->lasty, obs->lastx + obs->h, obs->y, obs->x + obs->h, ball->radius, obs->w);
//             if (!isnan(angle)){
//                 SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION , "BOUNCE! while going up with %s", obs->name);
//                 bounceoff(&ball->vy, &ball->vx, obs->vx, obs->vy, angle, false);
//                 hasBounced=true;
//             }
//         } else {
//             //collision while going down
//             //if (check_line_cross(ball->y + 2*ball->radius, ball->x, ball->newy + 2*ball->radius, ball->newx, obs->lasty, obs->lastx, obs->y, obs->x, 2*ball->radius, obs->w)){
//             angle = check_circle_cross_line(ball->y, ball->x, ball->newy, ball->newx, obs->lasty, obs->lastx, obs->y, obs->x, ball->radius, obs->w);
//             if (!isnan(angle)){
//                 SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION , "BOUNCE! while going down with %s", obs->name);
//                 bounceoff(&ball->vy, &ball->vx, obs->vx, obs->vy, angle, true);
//                 hasBounced=true;
//             }
//         }
//         if (isnan(angle)){
//             if (ball->x >= obs->lastx){
//                 //collision while going right
//                 //if (check_line_cross(ball->x, ball->y, ball->newx, ball->newy, obs->lastx + obs->w, obs->lasty, obs->x + obs->w, obs->y, 2*ball->radius, obs->h)){
//                 angle = check_circle_cross_line(ball->x, ball->y, ball->newx, ball->newy, obs->lastx + obs->w, obs->lasty, obs->x + obs->w, obs->y, ball->radius, obs->h);
//                 if (!isnan(angle)){
//                     SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION , "BOUNCE! while going right with %s", obs->name);
//                     bounceoff(&ball->vx, &ball->vy, obs->vy, obs->vx, angle, false);
//                     hasBounced=true;
//                 }
//             } else {
//                 //collision while going left
//                 //if (check_line_cross(ball->x + 2*ball->radius, ball->y, ball->newx + 2*ball->radius, ball->newy, obs->lastx, obs->lasty, obs->x, obs->y, 2*ball->radius, obs->h)){ 
//                 angle = check_circle_cross_line(ball->x, ball->y, ball->newx, ball->newy, obs->lastx, obs->lasty, obs->x, obs->y, ball->radius, obs->h);
//                 if (!isnan(angle)){ 
//                     SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION , "BOUNCE! while going left with %s", obs->name);
//                     bounceoff(&ball->vx, &ball->vy, obs->vy, obs->vx, angle, true);
//                     hasBounced=true;
//                 }
//             }
//         }
//     }
//     return hasBounced;
// }

//TODO fix flips for angle tangential speed change
void bounceoff(int* vnorm, int* vtan, int collider_vtan, int collider_vnorm, double angle, bool shouldFlipAgain){
    double complement = SDL_sqrt(1 - angle * angle);
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "vnorm: %d, vtan: %d (angle: %f complement %f)", *vnorm, *vtan, angle, complement);
    int shouldFlip;
    if (SDL_abs(*vtan) > 0 && SDL_abs(angle) > 0.0001)
        shouldFlip = (*vtan / SDL_abs(*vtan)) * (angle / SDL_abs(angle)) * (shouldFlipAgain ? 1 : 1);
    else shouldFlip = 1;
    *vtan = *vtan * (complement*shouldFlip) + collider_vtan / 2 * (complement) + SDL_abs(*vnorm) * SDL_abs(angle) * shouldFlip;
    if (SDL_abs(*vtan) > abs(MAX_BALL_SPEED)) 
        *vtan = (*vtan >= 0) ? MAX_BALL_SPEED : -MAX_BALL_SPEED;

    *vnorm = -(*vnorm) * complement + collider_vnorm;
    if (SDL_abs(*vnorm) > abs(MAX_BALL_SPEED)) 
        *vnorm = (*vnorm >= 0) ? MAX_BALL_SPEED : -MAX_BALL_SPEED;
}

void draw_ball(SDL_Renderer* renderer, ball_t* ball){
    SDL_Rect dest;

    dest.x = ball->x;
    dest.y = ball->y;
    dest.h = ball->radius*2;
    dest.w = ball->radius*2;
    
    blit(ball->texture, dest, ball->angle);
}

bool check_line_cross(int a1, int b1, int a2, int b2, int c1, int d1, int c2, int d2, int lenm, int lenfixed){
    //Computes if a line moving from (a1, b1) to (a2, b2) will cross obstacle at (c1, d1) moving to (c2, d2)
    if ((a1 < c1 && a2 >= c2) || (a1 > c1 && a2 <= c2)){
        //b1=475 b2=491 d1=200 d2=200
        if (b2 <= d2+lenfixed && b2+lenm >= d2){
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "A ((%d,%d)->(%d,%d) (%d) || (%d,%d)->(%d,%d) (%d))", a1, b1, a2, b2, lenm, c1, d1, c2, d2, lenfixed);
            return true;
        }
        if (b1 < d1+lenfixed && b2 >= d2+lenfixed){
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "B ((%d,%d)->(%d,%d) (%d) || (%d,%d)->(%d,%d) (%d))", a1, b1, a2, b2, lenm, c1, d1, c2, d2, lenfixed);
            return true;
        }
        if (b1 + lenm > d1 && b2 + lenm <= d2){
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "C ((%d,%d)->(%d,%d) (%d) || (%d,%d)->(%d,%d) (%d))", a1, b1, a2, b2, lenm, c1, d1, c2, d2, lenfixed);
            return true;
        }
    }
    return false;
}

double check_circle_cross_line(int ca1, int cb1, int ca2, int cb2, int la1, int lb1, int la2, int lb2, int rad, int ll){
    //Returns the sin of bounce angle (or NAN if not bounce, very unlikely sin)
    ca1 = ca1+rad;
    ca2 = ca2+rad;
    cb1 = cb1+rad;
    cb2 = cb2+rad;
    if ((ca1 + rad <= la1 && ca2+rad >= la2) || (ca1 - rad >= la1 && ca2 - rad <= la2) || (ca2 - rad >= la2 && ca2 + rad <= la2) || (ca2 + rad >= la2 && ca2 - rad <= la2)){
        if (cb2 >= lb2 && cb2 <= lb2 + ll) {    
            return 0; //final radius in range -> we collided with tip, its OK.
        } else {    //fuck, we have to get serious. i'm serious, i'm a serious... actor... VOICE ACTOR! V.A.!
            double vca = ((double)ca2 - ca1)/COLLIDER_PRECISION;    //Guaranteed to be the absolute
            double vcb = ((double)cb2 - cb1)/COLLIDER_PRECISION;    //worst derivative you've seen today!
            double vla = ((double)la2 - la1)/COLLIDER_PRECISION;    //If you've seen worse, call 0800-FUC-KYOU
            double vlb = ((double)lb2 - lb1)/COLLIDER_PRECISION;    //And we'll wire transfer you US$1B immediately
            for (int i = 0; i <= 100; i++){
                double ca = ca1 + vca*i;
                double cb = cb1 + vcb*i;
                double la = la1 + vla*i;
                double lb = lb1 + vlb*i;

                double deltaa = ca - la;
                double deltab = cb - lb;
                if (deltaa * deltaa + deltab * deltab <= rad*rad) {
                    double sin = -deltab / SDL_sqrt(deltaa * deltaa + deltab * deltab);
                    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "SIN%f", sin);
                    return sin;
                }
                lb = lb1 + ll + vlb*i;

                deltaa = ca - la;
                deltab = cb - lb;
                if (deltaa * deltaa + deltab * deltab <= rad*rad) {
                    double sin = -deltab / SDL_sqrt(deltaa * deltaa + deltab * deltab);
                    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "SIN%f", sin);
                    return sin;
                }
            }
        }
    } 
    return NAN; //improbable angle
}