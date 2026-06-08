#ifndef __COLLIDER_H
#define __COLLIDER_H

#include <list.h>

//Probably the worst event-driven collider you've seen today.
typedef enum collider_type{
    COLLIDER_CIRCLE,
    COLLIDER_RECT,
} collider_type;

typedef enum collider_priority_t{
    COLLIDER_PRIORITY_NORMAL,   //Normal
    COLLIDER_PRIORITY_HIGH,     //Will take priority in loop
    COLLIDER_PRIORITY_LOW,      //Will be turned off if stuck
    COLLIDER_PRIORITY_TRIGGER   //Only a trigger, does not affect movement
} collider_priority_t;

#define DEFAULT_MASS 1.0
#define VERY_HIGH_MASS 1000000000.0
#define COLL_LENIENCY_COEF 1.1
#define COLL_MAX_ATTEMPTS_PER_FRAME 100

typedef struct collider_t{
    double x;
    double y;
    double w;
    double h;
    double vx;
    double vy;
    double mass;
    collider_type type;
    collider_priority_t priority;
    const char* name;
    void* target;
    collider_t* frame_ignore;
    collider_t* (*apply)(void* target, collider_t* collider, double dt);
} collider_t;

typedef struct collider_event_t{
    collider_t* a;
    collider_t* b;
    collider_t* newa;
    collider_t* newb;
    double phi; //internal to collider
    double dt;
} collider_event_t;

typedef enum side_t{
    SIDE_LEFT = 0,
    SIDE_RIGHT = 1,
    SIDE_TOP = 2,
    SIDE_BOTTOM = 3
} side_t;


collider_t* create_collider(double x, double y, double vx, double vy, 
    double w, double h, const char* name, double mass,
    collider_type type, collider_priority_t priority, void* target, 
    collider_t* (*apply)(void* target, collider_t* collider, double dt));
void free_collider(collider_t* collider);
void update_collider(list colliders, double dt);

collider_event_t* calculate_collision(collider_t* a, collider_t* b, double dt);
collider_event_t* collide_circle_circle(collider_t* a, collider_t* b, double dt);
collider_event_t* collide_circle_rect(collider_t* circle, collider_t* rect, double dt);
collider_event_t* collide_rect_rect(collider_t* a, collider_t* b, double dt);

#endif