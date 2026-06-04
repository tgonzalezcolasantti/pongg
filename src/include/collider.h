#ifndef __COLLIDER_H
#define __COLLIDER_H

//Probably the worst event-driven collider you've seen today.
typedef enum {
    COLLIDER_CIRCLE,
    COLLIDER_RECT_OUTER,
    COLLIDER_RECT_INNER,
} collider_type;

typedef struct collider_t{
    int x;
    int y;
    int lastx;
    int lasty;
    int w;
    int h;
    int vx;
    int vy;
    bool fixed;
    collider_type type;
    const char* name;
    const void* target;
    const void (*apply)(void* target, collider_t* collider);
    const collider_t* (*update)(void* target, collider_t* collider);
} collider_t;

typedef struct collider_event_t{
    collider_t* a;
    collider_t* b;
    int xa;
    int ya;
    int xb;
    int yb;
    double dt;
} collider_event_t;

collider_event_t* collide(collider_t* a, collider_t* b);
collider_event_t* collide_circle_circle(collider_t* a, collider_t* b, double dt);
bool collide_circle_square(collider_t* circle, collider_t* square, double dt);
bool collide_square_square(collider_t* a, collider_t* b, double dt);
void collider_test_c_c_c();
void draw_collider_test(collider_t* c, collider_event_t* event, double collidedBeforeAngle);

#endif