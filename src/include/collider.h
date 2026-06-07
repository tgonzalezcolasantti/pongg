#ifndef __COLLIDER_H
#define __COLLIDER_H

//Probably the worst event-driven collider you've seen today.
typedef enum {
    COLLIDER_CIRCLE,
    COLLIDER_RECT,
} collider_type;

typedef struct collider_t{
    double x;
    double y;
    double w;
    double h;
    double vx;
    double vy;
    double mass;
    bool fixed;
    collider_type type;
    const char* name;
    const void* target;
    const void (*apply)(const void* target, collider_t* collider);
    collider_t* (*update)(const void* target);
} collider_t;

typedef struct collider_event_t{
    collider_t* a;
    collider_t* b;
    collider_t* newa;
    collider_t* newb;
    double dt;
} collider_event_t;

collider_event_t* calculate_collision(collider_t* a, collider_t* b, double dt);
collider_event_t* collide_circle_circle(collider_t* a, collider_t* b, double dt);
collider_event_t* collide_circle_rect(collider_t* circle, collider_t* rect, double dt);
collider_event_t* collide_rect_rect(collider_t* a, collider_t* b, double dt);
void collider_test_c_c_c();
void draw_collider_test(collider_t* a, collider_t* b, collider_event_t* event, double dt);

#endif