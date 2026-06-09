// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
using namespace std;

#include <SDL_image.h>
#include <SDL_ttf.h>

#include <defs.h>
#include <loop.h>
#include <input.h>
#include <draw.h>
#include <ball.h>
#include <list.h>
#include <bar.h>
#include <collider.h>
#include <text.h>
#include <cmath>

list obstacles;
double global_dt = 0;

void init_collider(){
    obstacles = create_list();
}

collider_t* create_collider(
    double x, double y, double vx, double vy, 
    double w, double h, const char* name, double mass,
    collider_type type, collider_priority_t priority, void* target,
    collider_t* (*apply)(void*, collider_t*, double), collider_t* collider
){
    if (!collider) {
        collider = (collider_t*)malloc(sizeof(collider_t));
        collider->target_ignore = create_list();
        list_append(obstacles, collider);
    }
    collider->x = x;
    collider->y = y;
    collider->vx = vx;
    collider->vy = vy;
    collider->w = w;
    collider->h = h;
    collider->name = name;
    collider->type = type;
    collider->target = target;
    collider->apply = apply;
    collider->mass = mass;
    collider->priority = priority;
    return collider;
}

void free_collider(collider_t* collider){
    free(collider);
}

void recalculate_collider_positions(collider_t* collider, double dt){
    collider->x = collider->x + collider->vx * dt;
    collider->y = collider->y + collider->vy * dt;
}

void remove_ignore_target_single(collider_t* a, collider_t* b, bool withHPcascade){
    list_iterator iter = init_iterator(a->target_ignore, 0);
    bool removed = false;
    while(iterator_has_next(iter)){
        collider_target_t* target = (collider_target_t*)iterator_next(iter);
        if (target->target == b){
            iterator_remove(iter);
            SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Removed ignore for %s-%s (a-ll = %d)", a->name, b->name, list_length(a->target_ignore));
            removed = true;
            free(target);
        }
        else if (target->target->priority == COLLIDER_PRIORITY_HIGH && withHPcascade){
            SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Clearing HP-ignore for %s-%s", target->target->name, b->name);
            iterator_remove(iter);
            remove_ignore_target_single(target->target, a, false);
            free(target);
        }
    }
    if (!removed) SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Failed to remove collider %s ignore from %s", b->name, a->name);
    free_iterator(iter);
}

void remove_ignore_target(collider_t* a, collider_t* b){
    remove_ignore_target_single(a, b, true);
    remove_ignore_target_single(b, a, true);
}

void add_ignore_timeout(collider_t* a, collider_t* b){
    list_iterator iter = init_iterator(a->target_ignore, 0);
    while(iterator_has_next(iter)){
        collider_target_t* target = (collider_target_t*)iterator_next(iter);
        if (target->target == b || target->target == a){
            target->ogdt += 2*MAX_TIME_IGNORE;
        }
    }
    free_iterator(iter);
}

bool ignore_contains_target(collider_t* a, collider_t* b){
    list_iterator iter = init_iterator(a->target_ignore ,0);
    while(iterator_has_next(iter)){
        collider_target_t* target = (collider_target_t*)iterator_next(iter);
        if (target->target == b){
            SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Target has ignore %f-%f", global_dt, target->ogdt);
            if (global_dt > target->ogdt + MAX_TIME_IGNORE){
                remove_ignore_target(a, b);
                free_iterator(iter);
                return false;
            }
            free_iterator(iter);
            return true;
        }
    }
    free_iterator(iter);
    return false;
}

void add_ignore_target(collider_t* a, collider_t* b, double dt, double angle){
    if (!ignore_contains_target(a, b)){
        SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Added ignore for %s-%s", a->name, b->name);
        collider_target_t* target = (collider_target_t*)malloc(sizeof(collider_target_t));
        target->target = b;
        target->ogdt = dt;
        target->angle = angle;
        list_append(a->target_ignore, target);
        target = (collider_target_t*)malloc(sizeof(collider_target_t));
        target->target = a;
        target->ogdt = dt;
        target->angle = angle;
        list_append(b->target_ignore, target);
    }
}

double ignore_target_angle(collider_t* a, collider_t* b){
    list_iterator iter = init_iterator(a->target_ignore ,0);
    while(iterator_has_next(iter)){
        collider_target_t* target = (collider_target_t*)iterator_next(iter);
        if (target->target == b){
            free_iterator(iter);
            return target->angle;
        }
    }
    free_iterator(iter);
    return false;
}

double calculate_angle(double xa, double ya, double xb, double yb){
    double dx = xa - xb;
    double dy = ya - yb;
    return atan2(-dy, dx);
}

void update_collider(double dt){
    double total_dt = 0;
    collider_event_t* winning_event;
    collider_event_t* hp_event;
    int attempts = 0;
    bool disableLow = false;
    do{
        //We're gonna check every iterator pair and select the most recent collision
        //We'll apply it and advance everything up to that point
        //And repeat until we have no collisions left in this timeframe.
        winning_event = NULL;
        hp_event = NULL;
        if (attempts++ > COLL_MAX_ATTEMPTS_PER_FRAME / 2) disableLow = true;
        list_iterator itera = init_iterator(obstacles, 0);
        for (int i = 0; i < list_length(obstacles); i++){
            collider_t* a = (collider_t*)iterator_next(itera);
            list_iterator iterb = init_iterator(obstacles, i+1);
            while(iterb && iterator_has_next(iterb)){
                collider_t* b = (collider_t*)iterator_next(iterb);
                if (((a->priority != COLLIDER_PRIORITY_TRIGGER || b->priority != COLLIDER_PRIORITY_TRIGGER)) && 
                    ((disableLow && (a->priority != COLLIDER_PRIORITY_LOW && b->priority != COLLIDER_PRIORITY_LOW)) || !disableLow)
                ){
                    collider_event_t* temp = calculate_collision(a, b, (dt - total_dt) * COLL_LENIENCY_COEF);
                    if (temp){
                        if ((a->priority == COLLIDER_PRIORITY_HIGH || b->priority == COLLIDER_PRIORITY_HIGH) && ((hp_event && hp_event->dt > temp->dt) || !hp_event)){
                            if (hp_event){
                                free(hp_event->newa);
                                free(hp_event->newb);
                                free(hp_event);
                            }
                            hp_event = temp;
                        }   
                        else if (!winning_event) winning_event = temp;
                        else if (winning_event->dt > temp->dt){
                            free(winning_event->newa);
                            free(winning_event->newb);
                            free(winning_event);
                            winning_event = temp;
                        }
                    }
                }
            }
            free_iterator(iterb);
        }
        free_iterator(itera);
        if (hp_event){
            if(winning_event){
                free(winning_event->newa);
                free(winning_event->newb);
                free(winning_event);
            }
            winning_event = hp_event;
        }
        if (winning_event){
            list_remove(obstacles, winning_event->a);
            list_remove(obstacles, winning_event->b);
        }
        list_iterator refreshiter = init_iterator(obstacles, 0);
        while(iterator_has_next(refreshiter)){
            collider_t* c = (collider_t*)iterator_next(refreshiter);
            recalculate_collider_positions(c, winning_event ? winning_event->dt : dt);
            if (c->apply) c->apply(c->target, c, winning_event ? winning_event->dt : dt);
        }
        free_iterator(refreshiter);
        if (winning_event){
            winning_event->dt = SDL_min(winning_event->dt, dt - total_dt);
            total_dt += winning_event->dt;
            //SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Collision between %s and %s", winning_event->a->name, winning_event->b->name);
            if (winning_event->newa->priority == COLLIDER_PRIORITY_TRIGGER && winning_event->newb->priority == COLLIDER_PRIORITY_TRIGGER){
                free(winning_event->newa);
                free(winning_event->newb);
                free(winning_event);
            }
            else if (winning_event->newa->priority == COLLIDER_PRIORITY_TRIGGER){
                memcpy(winning_event->a, winning_event->newa, sizeof(collider_t));
                collider_t* newa = winning_event->a->apply ? winning_event->a->apply(winning_event->a->target, winning_event->a, winning_event->dt) : winning_event->a;
                if (newa) list_append(obstacles, newa);
                recalculate_collider_positions(winning_event->b, winning_event->dt);
                collider_t* newb = winning_event->b->apply ? winning_event->b->apply(winning_event->b->target, winning_event->b, winning_event->dt) : winning_event->b;
                list_append(obstacles, newb);
                free(winning_event->newa);
                free(winning_event->newb);
            } else if (winning_event->newb->priority == COLLIDER_PRIORITY_TRIGGER){
                memcpy(winning_event->b, winning_event->newb, sizeof(collider_t));
                collider_t* newb = winning_event->b->apply ? winning_event->b->apply(winning_event->b->target, winning_event->b, winning_event->dt) : winning_event->b;
                if (newb) list_append(obstacles, newb);
                recalculate_collider_positions(winning_event->a, winning_event->dt);
                collider_t* newa =  winning_event->a->apply ? winning_event->a->apply(winning_event->a->target, winning_event->a, winning_event->dt) : winning_event->a;
                list_append(obstacles, newa);
                free(winning_event->newa);
                free(winning_event->newb);
            } else {
                //SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Not a trigger!");
                memcpy(winning_event->a, winning_event->newa, sizeof(collider_t));
                memcpy(winning_event->b, winning_event->newb, sizeof(collider_t));
                collider_t* newa = winning_event->a->apply(winning_event->a->target, winning_event->a, winning_event->dt);
                collider_t* newb = winning_event->b->apply(winning_event->b->target, winning_event->b, winning_event->dt);
                add_ignore_target(newa, newb, global_dt + total_dt, calculate_angle(newa->x, newa->y, newb->x, newb->y));
                list_append(obstacles, newa);
                list_append(obstacles, newb);
                free(winning_event->newa);
                free(winning_event->newb);
            }
            free(winning_event);
            //SDL_Delay(1000);
        } else {
            total_dt = dt;
        }
    } while(winning_event && attempts < COLL_MAX_ATTEMPTS_PER_FRAME);
    global_dt += total_dt;
}

collider_t* collider_clone(collider_t* og){
    collider_t* copy = (collider_t*)malloc(sizeof(collider_t));
    memcpy(copy, og, sizeof(collider_t));
    return copy;
}

collider_event_t* calculate_collision(collider_t* a, collider_t* b, double dt){
    switch(a->type){
        case COLLIDER_CIRCLE:
            switch(b->type){
                case COLLIDER_CIRCLE:
                    return collide_circle_circle(a, b, dt);
                case COLLIDER_RECT:
                    return collide_circle_rect(a, b, dt);
                default: break;
            }
            break;
        case COLLIDER_RECT:
            switch(b->type){
                case COLLIDER_CIRCLE:
                    return collide_circle_rect(b, a, dt);
                case COLLIDER_RECT:
                    return collide_rect_rect(a, b, dt);
                default: break;
            }
            break;
        default: break;
    }
    return NULL;
}

quadrant_t angle_quadrant(double angle){
    angle = fmod(angle + 2*M_PI, 2*M_PI);
    if (angle >= M_PI/4 && angle < 3*M_PI/4) return FIRST_SECOND;
    if (angle >= 3*M_PI/4 && angle < 5*M_PI/4) return SECOND_THIRD;
    if (angle >= 5*M_PI/4 && angle < 7*M_PI/4) return THIRD_FOURTH;
    return FOURTH_FIRST;
}

void quadratic(double a, double b, double c, double* x1, double* x2){
    double disc = sqrt((b*b)-4*a*c);
    if (disc < 0 || abs(a) < 0.000001){
        *x1 = NAN;
        *x2 = NAN;
        return;
    } else {
        *x1 = (-b + disc) / (2*a);
        *x2 = (-b - disc) / (2*a);
    }
}

double smallest_positive(double a, double b){
    if (isnan(a)) return (isnan(b) || b < 0.0001) ? NAN : b;
    if (isnan(b)) return (isnan(a) || a < 0.0001) ? NAN : a;
    if (a >= -0.0001 && b >= -0.0001) return SDL_min(a, b);
    if (a < -0.0001 && b < -0.0001) return NAN;
    return SDL_max(a, b);
}

void recalculate_positions(collider_event_t* event){
    event->newa->x = event->a->x + event->a->vx * event->dt;
    event->newa->y = event->a->y + event->a->vy * event->dt;
    event->newb->x = event->b->x + event->b->vx * event->dt;
    event->newb->y = event->b->y + event->b->vy * event->dt;
}

void recalculate_velocities(collider_event_t* event){
    // based on https://en.wikipedia.org/wiki/Elastic_collision
    double va = sqrt(event->a->vx*event->a->vx + event->a->vy*event->a->vy);
    double vb = sqrt(event->b->vx*event->b->vx + event->b->vy*event->b->vy);
    double thetaa = calculate_angle(event->a->vx, event->a->vy, 0, 0);
    double thetab = calculate_angle(event->b->vx, event->b->vy, 0, 0);
    //SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "dt %f\tphi %f\ttheta1 %f\ttheta2 %f\tva %f\tvb %f", event->dt, event->phi*180/M_PI, thetaa*180/M_PI, thetab*180/M_PI, va, vb);
    event->newa->vx = (cos(event->phi) * (va * cos(thetaa - event->phi) * (event->a->mass - event->b->mass) + 2 * event->b->mass * vb * cos(thetab - event->phi)) / (event->a->mass + event->b->mass) + va * sin(thetaa - event->phi) * cos(event->phi + M_PI/2));
    event->newa->vy = -(sin(event->phi) * (va * cos(thetaa - event->phi) * (event->a->mass - event->b->mass) + 2 * event->b->mass * vb * cos(thetab - event->phi)) / (event->a->mass + event->b->mass) + va * sin(thetaa - event->phi) * sin(event->phi + M_PI/2));
    event->newb->vx = (cos(event->phi) * (vb * cos(thetab - event->phi) * (event->b->mass - event->a->mass) + 2 * event->a->mass * va * cos(thetaa - event->phi)) / (event->b->mass + event->a->mass) + vb * sin(thetab - event->phi) * cos(event->phi + M_PI/2));
    event->newb->vx = -(sin(event->phi) * (vb * cos(thetab - event->phi) * (event->b->mass - event->a->mass) + 2 * event->a->mass * va * cos(thetaa - event->phi)) / (event->b->mass + event->a->mass) + vb * sin(thetab - event->phi) * sin(event->phi + M_PI/2));
    //SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "New velocities: %s (%f, %f)->(%f, %f),\t%s (%f, %f)->(%f, %f)", event->newa->name, event->a->vx, event->a->vy, event->newa->vx, event->newa->vy, event->newb->name, event->b->vx, event->b->vy, event->newb->vx, event->newb->vy);
}

collider_event_t* collide_circle_circle(collider_t* a, collider_t* b, double dt){
    //Circle formula is x^2 + y^2 = R^2 for a point on the edge.
    //We use that to calculate how long it would take for 2 circles to collide.
    //It expands using x = (xa + vxa*t - xb - vxb*t) (and the same for Y).
    //Then uses a regular quad solver to get possible times.
    //And if it happens inside this frame, we use that (else we didn'y collide)

    //Calculate centers (collider gives corner point)
    double xa = a->x + a->w/2;
    double ya = a->y + a->h/2;
    double xb = b->x + b->w/2;
    double yb = b->y + b->h/2;
    
    //calculates positional and velocity differences
    double dx = xa - xb;
    double dy = ya - yb;
    double dvx = a->vx - b->vx;
    double dvy = a->vy - b->vy;

    //calculates coefficients from replacing dx and such in the circle formula
    double ca = dvx * dvx + dvy * dvy;
    double cb = 2 * (dx * dvx + dy * dvy);
    double cc = dx*dx + dy*dy - (a->w/2 + b->w/2) * (a->h/2 + b->h/2);

    double dt_ans = NAN;
    double dt1, dt2;
    quadratic(ca, cb, cc, &dt1, &dt2);
    dt_ans = smallest_positive(dt1, dt2);

    if (!isnan(dt_ans) && dt_ans <= dt){  //Quadratic says they'll collide soon enough to warrant acting now.
        collider_event_t* event = (collider_event_t*)malloc(sizeof(collider_event_t));
        event->a = a;
        event->b = b;
        event->dt = SDL_min(dt_ans, dt);
        event->newa = collider_clone(a);
        event->newb = collider_clone(b);
        recalculate_positions(event);
        event->phi = calculate_angle(event->newa->x + a->w/2.0, event->newa->y + a->h/2.0, event->newb->x + b->w/2.0, event->newb->y + b->h/2.0);
        recalculate_velocities(event);
        return event;
    }
    return NULL;
}

double collide_rect_line(double ln, double lt, double ltlen, double sqn, double sqt, double sqnlen, double sqtlen, double vn, double vt, double sqvn, double sqvt){
    //Calculates a collission between a rectangle and a line. 
    //Parameters use normal and tangent instead of x and y so we can use the same funcion for x and y-bound lines

    /*
  (ln, lt)      (sqn, sqt)  
    |               |-------|
    | -->           |       |
    |               |_______|
  (ln, lt+ltlen)        (sqn+sqnlen, sqt+sqtlen)
    */
    //Conditions: t/ ln(t) - sqn(t) = 0     ->      ln + vln*t - (sqn - sqvn * t) = 0 -> ln - sqn = (sqvn - vln)*t -> t=(ln-sqn)/(sqvn-vln)
    //aka we are aligned on n between these times   ln+vln*t - (sqn+sqnlen-sqvn*t) = 0 -> ln - sqn - sqnlen = (sqvn-vln)*t -> t=(ln-sqn-sqnlen)/(sqvn-vln)

    //Also lt>=sqt+sqtlen during this   -cutoff->   lt + vt*t = sqt + sqtlen + sqvt*t -> lt - sqt - sqtlen = (sqvt - vt)*t -> t = (lt-sqt-sqtlen)/(sqvt-vt)
    //And  lt+ltlen<=sqt as well            ->      lt + ltlen + vt*t = sqt + sqvt*t -> lt + ltlen - sqt = (sqvt - vt)*t  -> t = (lt+ltlen-sqt)/(sqvt-vt)
    
    //The idea is we calculate the times for x and then use the y times to see if we have a transition, and then check y conditions at regions
    //simpler y conditions: lt-sqt-sqtlen+vt*t-sqvt*t>=0
    //                      sqt-lt-ltlen+sqvt*t-vt*t >=0

    double dt_n1 = (ln + sqnlen - sqn) / (sqvn - vn);
    double dt_n2 = (ln - sqn) / (sqvn - vn);
    double dt_t1 = (lt - sqt - sqtlen) / (sqvt - vt);
    double dt_t2 = (lt + ltlen - sqt) / (sqvt - vt);
    
    //if n1 is nan, so is n2. same with t
    if (!isnan(dt_n1)){
        double min_dtn = smallest_positive(dt_n1, dt_n2);
        bool min_dt_condition = (lt - sqt - sqtlen+ vt*min_dtn - sqvt*min_dtn >= -0.0001 && 
            sqt - lt - ltlen + sqvt*min_dtn - vt*min_dtn >= -0.0001);

        //case 1: no t times -> we check conditions at min time
        //case 2: t times outside of n time range -> we check conditions at min time
        //case 2: one time in range -> We have 2 regions to check
        //BUT must transition + -> - or - -> + only, so we check at min and simply return min or dt appropiately
        //case 3: both times in range -> transition is still + -> - so we can only be on 1st or 2nd region, never 3rd. 
        
        //All of these cases can be covered with this
        double dt_t = smallest_positive(dt_t1, dt_t2); //either none are nan or both are nan, so its ok
        return min_dt_condition ? min_dtn : (dt_t <= SDL_max(dt_n1, dt_n2) ? dt_t : NAN);
    }
    return NAN;
}

collider_event_t* collide_rect_rect(collider_t* a, collider_t* b, double dt){
    //Calculates a collision between two rectangles a & b by dividing a into lines and
    //calculating those collisions. Will fail if b is fully inside a, but that should
    //never happen and be relevant to detect so we're good i guess.

    //Will calculate the best collision time and use that for the final result.
    collider_event_t* event = (collider_event_t*)malloc(sizeof(collider_event_t));
    event->a = a;
    event->b = b;
    event->newa = collider_clone(a);
    event->newb = collider_clone(b);
    event->dt = NAN;

    //line a-left on b
    double temp = collide_rect_line(a->x, a->y, a->h, b->x, b->y, b->w, b->h, a->vx, a->vy, b->vx, b->vy);
    if (!isnan(temp) && temp >= -0.0001 && (isnan(event->dt) || temp <= event->dt)){
        event->dt = temp;
        event->phi = 0;
    }
    //line a-right on b
    temp = collide_rect_line(a->x + a->w, a->y, a->h, b->x, b->y, b->w, b->h, a->vx, a->vy, b->vx, b->vy);
    if (!isnan(temp) && temp >= -0.0001 && (isnan(event->dt) || temp <= event->dt)){
        event->dt = temp;
        event->phi = M_PI;
    }
    //line a-top on b
    temp = collide_rect_line(a->y, a->x, a->w, b->y, b->x, b->h, b->w, a->vy, a->vx, b->vy, b->vx);
    if (!isnan(temp) && temp >= -0.0001 && (isnan(event->dt) || temp <= event->dt)){
        event->phi = M_PI/2;
        event->dt = temp;
    }
    //line a-bottom on b
    temp = collide_rect_line(a->y + a->h, a->x, a->w, b->y, b->x, b->h, b->w, a->vy, a->vx, b->vy, b->vx);
    if (!isnan(temp) && temp >= -0.0001 && (isnan(event->dt) || temp <= event->dt)){
        event->dt = temp;
        event->phi = -M_PI/2;
    }
    if (!isnan(event->dt) && event->dt >= 0 && event->dt <= dt){
        event->dt = SDL_min(event->dt, dt);
        recalculate_positions(event);
        recalculate_velocities(event);
        return event;
    }

    free(event->newa);
    free(event->newb);
    free(event);
    return NULL;
}

double collide_circle_point(double cn, double ct, double r, double vcn, double vct, double ln, double lt, double vln, double vlt){
    double t1, t2; //temp variables

    //First we check if any points belong in circle

    //Point a
    double dn = cn - ln;
    double dt = ct - lt;
    double dvn = vcn - vln;
    double dvt = vct - vlt;

    double ca = dvn * dvn + dvt * dvt;
    double cb = 2 * (dn * dvn + dt * dvt);
    double cc = dn*dn + dt*dt - r*r;
    quadratic(ca, cb, cc, &t1, &t2);
    return smallest_positive(t1, t2);
}

double collide_circle_line(double cn, double ct, double r, double vcn, double vct, double ln, double lt, double ll, double vln, double vlt){
    //Line
    //check if cn in range
    //cn + vcn*t - (ln + vln*t) = R -> cn-ln-R = (vln-vcn)*t -> t = (cn-ln-R)/(vln-vcn)
    double dt_line = smallest_positive(((double)cn-ln-r)/(vln-vcn), ((double)cn-ln+r)/(vln-vcn));
    if (!isnan(dt_line)){
        //May be valid so lets check
        if(ct + vct*dt_line >= lt + vlt*dt_line && ct + vct*dt_line <= lt + ll + vlt*dt_line) return dt_line;
    }
    return NAN;
}

double collide_circle_rect_side(collider_event_t* event, side_t side){
    //TOOD Positions for angle stuff.
    //circle on rectangle-left
    bool flipSides = side == SIDE_LEFT || side == SIDE_RIGHT;
    bool addLen = side == SIDE_RIGHT || side == SIDE_BOTTOM;
    double cn = flipSides ? (event->a->x + event->a->w/2) : (event->a->y + event->a->h/2);
    double ct = flipSides ? (event->a->y + event->a->h/2) : (event->a->x + event->a->w/2);
    double vcn = flipSides ? (event->a->vx) : (event->a->vy);
    double vct = flipSides ? (event->a->vy) : (event->a->vx);
    double ln = flipSides ? (event->b->x + (addLen ? event->b->w : 0)) : (event->b->y + (addLen ? event->b->h : 0));
    double lt = flipSides ? (event->b->y) : (event->b->x);
    double vln = flipSides ? (event->b->vx) : (event->b->vy);
    double vlt = flipSides ? (event->b->vy) : (event->b->vx);
    double ll = flipSides ? (event->b->h) : (event->b->w);

    double temp = collide_circle_point(cn, ct, event->a->h/2, vcn, vct, ln, lt, vln, vlt);
    if (!isnan(temp) && temp >= -0.0001 && (isnan(event->dt) || temp <= event->dt)){
        event->dt = temp;
        recalculate_positions(event);
        double bx, by;
        switch(side){
            case SIDE_LEFT:
            case SIDE_TOP:
                bx = event->newb->x;
                by = event->newb->y;
                break;
            case SIDE_RIGHT:
                bx = event->newb->x + event->newb->w;
                by = event->newb->y;
                break;
            case SIDE_BOTTOM:
                bx = event->newb->x;
                by = event->newb->y + event->newb->h;
                break;
        }
        event->phi = calculate_angle(event->newa->x + event->newa->w/2, event->newa->y + event->newa->h/2, bx, by);
    }
    temp = collide_circle_point(cn, ct, event->a->h/2, vcn, vct, ln, lt + ll, vln, vlt);
    if (!isnan(temp) && temp >= -0.0001 && (isnan(event->dt) || temp <= event->dt)){
        event->dt = temp;
        recalculate_positions(event);
        double bx, by;
        switch(side){
            case SIDE_LEFT:
                bx = event->newb->x;
                by = event->newb->y + event->newb->h;
            case SIDE_TOP:
                bx = event->newb->x + event->newb->w;
                by = event->newb->y;
                break;
            case SIDE_RIGHT:
            case SIDE_BOTTOM:
                bx = event->newb->x + event->newb->w;
                by = event->newb->y + event->newb->h;
                break;
        }        
        event->phi = calculate_angle(event->newa->x + event->newa->w/2, event->newa->y + event->newa->h/2, bx, by);    
    }
    temp = collide_circle_line(cn, ct, event->a->h/2.0, vcn, vct, ln, lt, ll, vln, vlt);
    if (!isnan(temp) && temp >= -0.0001 && (isnan(event->dt) || temp < event->dt)){
        event->dt = temp;
        recalculate_positions(event);
        switch(side){
            case SIDE_LEFT: event->phi=0; break;
            case SIDE_RIGHT: event->phi=M_PI; break;
            case SIDE_TOP: event->phi=M_PI/2; break;
            case SIDE_BOTTOM: event->phi=3*M_PI/2; break;
            default: break;
        }
    }
    return event->dt;
}

collider_event_t* collide_circle_rect(collider_t* circle, collider_t* rect, double dt){
    //Calculates a collision between circle and rectangle by dividing rectangle into lines and
    //calculating those collisions. Will fail if circle is fully inside rectangle, but that should
    //never happen and be relevant to detect so we're good i guess.
    //Actually that's a good thing tbh, we can detect inner and outer collisions the exact same way then.

    collider_event_t* event = (collider_event_t*)malloc(sizeof(collider_event_t));
    event->a = circle;
    event->b = rect;
    event->dt = NAN;
    event->newa = collider_clone(circle);
    event->newb = collider_clone(rect);

    collide_circle_rect_side(event, SIDE_LEFT);
    collide_circle_rect_side(event, SIDE_RIGHT);
    collide_circle_rect_side(event, SIDE_TOP);
    collide_circle_rect_side(event, SIDE_BOTTOM);
    if (!isnan(event->dt) && event->dt >= 0 && event->dt <= dt){
        recalculate_positions(event);
        recalculate_velocities(event);
        if (ignore_contains_target(circle, rect) && angle_quadrant(ignore_target_angle(circle, rect)) == angle_quadrant(calculate_angle(event->newa->x, event->newa->y, event->newb->x, event->newb->y))){
            if (event->dt < MAX_TIME_IGNORE) {
                add_ignore_timeout(circle, rect);
            }
            free(event->newa);
            free(event->newb);
            free(event);
            return NULL;
        }
        //SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Updated %s: p(%f, %f), v(%f, %f)", event->newa->name, event->newa->x, event->newa->y, event->newa->vx, event->newa->vy);
        //SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Updated %s: p(%f, %f), v(%f, %f)", event->newb->name, event->newb->x, event->newb->y, event->newb->vx, event->newb->vy);
        return event;
    }
    free(event->newa);
    free(event->newb);
    free(event);
    return NULL;
}