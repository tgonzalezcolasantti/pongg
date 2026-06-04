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
#include <bar.h>
#include <collider.h>
#include <text.h>

collider_t* init_collider(
    collider_t* collider, 
    int x, int y, int vx, int vy, 
    int w, int h, const char* name, bool isfixed, 
    collider_type type, const void* target, 
    const void (*apply)(void* target, collider_t* collider), 
    const collider_t* (*update)(void* target, collider_t* collider)
){
    collider->x = x;
    collider->y = y;
    collider->lastx = x;
    collider->lasty = y;
    collider->vx = vx;
    collider->vy = vy;
    collider->w = w;
    collider->h = h;
    collider->fixed = isfixed;
    collider->name = name;
    collider->type = type;
    collider->target = target;
    collider->apply = apply;
    collider->update = update;
    return collider;
}

collider_event_t* calculate_collision(collider_t* a, collider_t* b, double dt){
    switch(a->type){
        case COLLIDER_CIRCLE:
            switch(b->type){
                case COLLIDER_CIRCLE:
                    return collide_circle_circle(a, b, dt);
                // default:
                //     return collide_circle_square(a, b, dt);
            }
        // default:
        //     switch(b->type){
        //         case COLLIDER_CIRCLE:
        //             return collide_circle_square(b, a, dt);
        //         default:
        //             return collide_square_square(a, b, dt);
        //     }
    }
}

double calculate_collision_angle(collider_event_t* c){
    if (!c) return NAN;
    double dx = (c->xa + c->a->vx * c->dt) - (c->xb + c->b->vx * c->dt);
    double dy = (c->ya + c->a->vy * c->dt) - (c->yb + c->b->vy * c->dt);
    return SDL_atan(dy/(SDL_abs(dx) < 0.00001 ? 0.00001 * (dx > 0 ? 1 : -1) : dx));
}

void quadratic(double a, double b, double c, double* x1, double* x2){
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "%f %f %f", a, b, c);
    double disc = SDL_sqrt((b*b)-4*a*c);
    if (disc < 0 || SDL_abs(a) < 0.000001){
        *x1 = NAN;
        *x2 = NAN;
        return;
    } else {
        *x1 = (-b + disc) / (2*a);
        *x2 = (-b - disc) / (2*a);
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "%f %f", *x1, *x2);
    }
}

double smallest_positive(double a, double b){
    if (isnan(a)) return (isnan(b) || b < 0) ? NAN : b;
    if (isnan(b)) return (isnan(a) || a < 0) ? NAN : a;
    if (a >= 0 && b >= 0) return SDL_min(a, b);
    if (a < 0 && b < 0) return NAN;
    return SDL_max(a, b);
}

double collide_circle_circle_now(collider_t* a, collider_t* b){
    double xa = a->x + a->w/2;
    double ya = a->y + a->h/2;
    double xb = b->x + b->w/2;
    double yb = b->y + b->h/2;
    
    double dx = xa - xb;
    double dy = ya - yb;
    if (SDL_sqrt(dx*dx + dy*dy) <= a->w/2.0 + b->w/2.0){
        return SDL_atan(dy/(SDL_abs(dx) < 0.00001 ? 0.00001 * (dx > 0 ? 1 : -1) : dx));
    } return NAN;
}

collider_event_t* collide_circle_circle(collider_t* a, collider_t* b, double dt){
    //Circle formula is x^2 + y^2 = R^2 for a point on the edge.
    //We use that to calculate how long it would take for 2 circles to collide.
    //And if it happens inside this frame, we use that (else we didn'y collide)
    double xa = a->x + a->w/2.0;
    double ya = a->y + a->h/2.0;
    double xb = b->x + b->w/2.0;
    double yb = b->y + b->h/2.0;
    
    double dx = xa - xb;
    double dy = ya - yb;
    double dvx = a->vx - b->vx;
    double dvy = a->vy - b->vy;

    double ca = dvx * dvx + dvy * dvy;
    double cb = 2 * (dx * dvx + dy * dvy);
    double cc = dx*dx + dy*dy - (a->w/2.0 + b->w/2.0) * (a->h/2.0 + b->h/2.0);

    double dt_ans = NAN;
    // if (SDL_abs(ca) <= 0.0001){
    //     return collide_circle_circle_now(a, b); //Not colliding soon, so return if colliding right now.
    //} else {
    double dt1, dt2;
    quadratic(ca, cb, cc, &dt1, &dt2);
    dt_ans = smallest_positive(dt1, dt2);
    //}
    if (!isnan(dt_ans) && dt_ans <= dt){  //Quadratic says they'll collide soon enough to warrant acting now.
        collider_event_t* event = (collider_event_t*)SDL_malloc(sizeof(collider_event_t));
        event->a = a;
        event->b = b;
        event->dt = dt;
        event->xa=xa;
        event->xb=xb;
        event->ya=ya;
        event->yb=yb;
        return event;
    }
    return NULL;
}

double collide_square_point(int x, int y, int sx, int sy, int sw, int sh, int vx, int vy, int svx, int svy){
    double dt_x1 = (double)(x + sw - sx) / (svx - vx);
    double dt_x2 = (double)(x + sw - sx - sw) / (svx - vx);
    double dt_y1 = (double)(y + sh - sy) / (svy - vy);
    double dt_y2 = (double)(y + sh - sy - sh) / (svy - vy);

    //if x1 is nan, so is x2. same with y
    if(!isnan(dt_x1) && !isnan(dt_y1)){
        if (dt_x1 >= 0 && dt_x1 >= dt_y1 && dt_x1 <= dt_y2){
            return dt_x1;
        }
        if (dt_y1 >= 0 && dt_y1 >= dt_x1 && dt_y1 <= dt_y2){
            return dt_y1;
        }
        if (dt_x2 >= 0 && dt_x2 >= dt_y1 && dt_x2 <= dt_y2){
            return dt_x2;
        }
        if (dt_y2 >= 0 && dt_y2 >= dt_x1 && dt_y2 <= dt_y2){
            return dt_y2;
        }
    }
    return NAN;
}

bool collide_square_square(collider_t* a, collider_t* b){
    double dts[4] = {NAN};
}

void collider_test_c_c_c(){
    input_t input;
    init_text();
    init_input(&input);
    collider_t a;
    collider_t b;
    //case 1 -> No collision
    init_collider(&a, 0, 0, 0, 0, 200, 200, "a", false, COLLIDER_CIRCLE, NULL, NULL, NULL);
    init_collider(&b, 200, 200, 0, 0, 200, 200, "b", false, COLLIDER_CIRCLE, NULL, NULL, NULL);
    prepareScene(load_texture(ASSET_WALL));
    presentScene();
    SDL_Delay(100);

    collider_event_t* collided = collide_circle_circle(&a, &b, 1);
    double collidedBefore = collide_circle_circle_now(&a, &b);
    prepareScene(load_texture(ASSET_WALL));
    draw_collider_test(&a, collided, collidedBefore);
    draw_collider_test(&b, collided, collidedBefore);
    SDL_Texture* text = getTextTexture("1 (nocoll)");
    SDL_SetTextureColorMod(text, 0, 0, 0);
    easyblit(text, 0, 800);
    presentScene();
    SDL_Delay(1000);

    //case 2 -> overlap
    a.x += 40;
    a.y += 40;
    b.x -= 40;
    b.y -= 40;
    collided = collide_circle_circle(&a, &b, 1);
    collidedBefore = collide_circle_circle_now(&a, &b);
    prepareScene(load_texture(ASSET_WALL));
    draw_collider_test(&a, collided, collidedBefore);
    draw_collider_test(&b, collided, collidedBefore);
    text = getTextTexture("2 (fullcoll)");
    SDL_SetTextureColorMod(text, 0, 0, 0);
    easyblit(text, 0, 800);
    presentScene();
    SDL_Delay(1000);

    //case 3 -> graze
    a.x = 0;
    a.y = 0;
    b.x = 141;
    b.y = 141;
    collided = collide_circle_circle(&a, &b, 1);
    collidedBefore = collide_circle_circle_now(&a, &b);
    prepareScene(load_texture(ASSET_WALL));
    draw_collider_test(&a, collided, collidedBefore);
    draw_collider_test(&b, collided, collidedBefore);
    text = getTextTexture("3 (graze)");
    SDL_SetTextureColorMod(text, 0, 0, 0);
    easyblit(text, 0, 800);
    presentScene();
    SDL_Delay(1000);

    //case 4 -> antigraze
    a.x = 0;
    a.y = 0;
    b.x = 142;
    b.y = 142;
    collided = collide_circle_circle(&a, &b, 1);
    collidedBefore = collide_circle_circle_now(&a, &b);
    prepareScene(load_texture(ASSET_WALL));
    draw_collider_test(&a, collided, collidedBefore);
    draw_collider_test(&b, collided, collidedBefore);
    text = getTextTexture("4 (antigraze)");
    SDL_SetTextureColorMod(text, 0, 0, 0);
    easyblit(text, 0, 800);
    presentScene();
    SDL_Delay(1000);

    //case 5 -> overlap moving x
    a.x = 0;
    a.y = 0;
    a.vx = 100;
    a.vy = 0;
    b.x = 250;
    b.y = 0;
    b.vx = 0;
    b.vy = 0;
    collided = collide_circle_circle(&a, &b, 1);
    collidedBefore = collide_circle_circle_now(&a, &b);
    prepareScene(load_texture(ASSET_WALL));
    draw_collider_test(&a, collided, collidedBefore);
    draw_collider_test(&b, collided, collidedBefore);
    text = getTextTexture("5 (overlap moving x)");
    SDL_SetTextureColorMod(text, 0, 0, 0);
    easyblit(text, 0, 800);
    presentScene();
    SDL_Delay(1000);

    //case 6 -> overlap moving x
    a.x = 0;
    a.y = 0;
    a.vy = 100;
    a.vx = 0;
    b.y = 250;
    b.x = 0;
    b.vx = 0;
    b.vy = 0;
    collided = collide_circle_circle(&a, &b, 1);
    collidedBefore = collide_circle_circle_now(&a, &b);
    prepareScene(load_texture(ASSET_WALL));
    draw_collider_test(&a, collided, collidedBefore);
    draw_collider_test(&b, collided, collidedBefore);
    text = getTextTexture("6 (overlap moving y)");
    SDL_SetTextureColorMod(text, 0, 0, 0);
    easyblit(text, 0, 800);
    presentScene();
    SDL_Delay(1000);

    //case 7 -> overlap moving mult
    a.x = 0;
    a.y = 0;
    a.vy = 100;
    a.vx = 100;
    b.y = 250;
    b.x = 250;
    b.vx = -50;
    b.vy = -50;
    collided = collide_circle_circle(&a, &b, 1);
    collidedBefore = collide_circle_circle_now(&a, &b);
    prepareScene(load_texture(ASSET_WALL));
    draw_collider_test(&a, collided, collidedBefore);
    draw_collider_test(&b, collided, collidedBefore);
    text = getTextTexture("7 (overlap moving)");
    SDL_SetTextureColorMod(text, 0, 0, 0);
    easyblit(text, 0, 800);
    presentScene();
    SDL_Delay(1000);

    //case 8 -> ghost
    a.x = 0;
    a.y = 0;
    a.vx = 500;
    a.vy = 0;
    b.x = 250;
    b.y = 0;
    b.vx = 0;
    b.vy = 0;
    collided = collide_circle_circle(&a, &b, 1);
    prepareScene(load_texture(ASSET_WALL));
    collidedBefore = collide_circle_circle_now(&a, &b);
    draw_collider_test(&a, collided, collidedBefore);
    draw_collider_test(&b, collided, collidedBefore);
    text = getTextTexture("8 (ghost-x)");
    SDL_SetTextureColorMod(text, 0, 0, 0);
    easyblit(text, 0, 800);
    presentScene();
    SDL_Delay(1000);

    //case 9 -> ghost
    a.x = 0;
    a.y = 0;
    a.vx = 0;
    a.vy = 500;
    b.x = 0;
    b.y = 250;
    b.vx = 0;
    b.vy = 0;
    collided = collide_circle_circle(&a, &b, 1);
    prepareScene(load_texture(ASSET_WALL));
    collidedBefore = collide_circle_circle_now(&a, &b);
    draw_collider_test(&a, collided, collidedBefore);
    draw_collider_test(&b, collided, collidedBefore);
    text = getTextTexture("9 (ghost-y)");
    SDL_SetTextureColorMod(text, 0, 0, 0);
    easyblit(text, 0, 800);
    presentScene();
    SDL_Delay(1000);

    //case 10 -> ghost
    a.x = 0;
    a.y = 0;
    a.vx = 500;
    a.vy = 500;
    b.x = 200;
    b.y = 200;
    b.vx = 50;
    b.vy = 50;
    collided = collide_circle_circle(&a, &b, 1);
    prepareScene(load_texture(ASSET_WALL));
    collidedBefore = collide_circle_circle_now(&a, &b);
    draw_collider_test(&a, collided, collidedBefore);
    draw_collider_test(&b, collided, collidedBefore);
    text = getTextTexture("10 (ghost-traverse)");
    SDL_SetTextureColorMod(text, 0, 0, 0);
    easyblit(text, 0, 800);
    presentScene();
    SDL_Delay(1000);

    //case 11 -> ghost-traverse overlap
    a.x = 0;
    a.y = 0;
    a.vx = 500;
    a.vy = 500;
    b.x = 500;
    b.y = 500;
    b.vx = 100;
    b.vy = 100;
    collided = collide_circle_circle(&a, &b, 1);
    prepareScene(load_texture(ASSET_WALL));
    collidedBefore = collide_circle_circle_now(&a, &b);
    draw_collider_test(&a, collided, collidedBefore);
    draw_collider_test(&b, collided, collidedBefore);
    text = getTextTexture("11 (ghost-traverse overlap)");
    SDL_SetTextureColorMod(text, 0, 0, 0);
    easyblit(text, 0, 800);
    presentScene();
    SDL_Delay(1000);

    //case 12 -> ghost-traverse no overlap
    a.x = 0;
    a.y = 0;
    a.vx = 500;
    a.vy = 500;
    b.x = 500;
    b.y = 500;
    b.vx = 150;
    b.vy = 150;
    collided = collide_circle_circle(&a, &b, 1);
    prepareScene(load_texture(ASSET_WALL));
    collidedBefore = collide_circle_circle_now(&a, &b);
    draw_collider_test(&a, collided, collidedBefore);
    draw_collider_test(&b, collided, collidedBefore);
    text = getTextTexture("12 (ghost-traverse no overlap)");
    SDL_SetTextureColorMod(text, 0, 0, 0);
    easyblit(text, 0, 800);
    presentScene();
    SDL_Delay(1000);
}

void draw_collider_test(collider_t* c, collider_event_t* event, double collidedBeforeAngle){
    SDL_Texture* colltext;
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Angle %f", calculate_collision_angle(event));
    if (c->type == COLLIDER_CIRCLE) colltext = load_texture(ASSET_CIRCLE_DEBUG);
    else colltext = load_texture(ASSET_SQUARE_DEBUG);

    if (!isnan(collidedBeforeAngle)) SDL_SetTextureColorMod(colltext, 0, 255, 0);
    else SDL_SetTextureColorMod(colltext, 0, 0, 0);
    SDL_SetTextureAlphaMod(colltext, 50);
    blit(colltext, {c->x, c->y, c->w, c->h}, 0);

    if (!isnan(calculate_collision_angle(event))) SDL_SetTextureColorMod(colltext, 0, 255, 0);
    else SDL_SetTextureColorMod(colltext, 255, 0, 0);
    SDL_SetTextureAlphaMod(colltext, 255);
    blit(colltext, {c->x + c->vx, c->y + c->vy, c->w, c->h}, 0);    

    SDL_Texture* name_text = getTextTexture((char*)c->name);
    SDL_SetTextureColorMod(name_text, 0, 0, 0);
    SDL_SetTextureAlphaMod(name_text, 50);
    blit(name_text, {c->x + c->w/2-20, c->y + c->h/2 -20, 40, 40}, !isnan(collidedBeforeAngle) ? collidedBeforeAngle * 180 / 3.14 : 0);
    SDL_SetTextureColorMod(name_text, 0, 0, 0);
    SDL_SetTextureAlphaMod(name_text, 255);
    blit(name_text, {c->x + c->vx + c->w/2-20, c->y + c->vy + c->h/2 -20, 40, 40}, !isnan(calculate_collision_angle(event)) ? calculate_collision_angle(event) * 180 / 3.14 : 0);
    
}