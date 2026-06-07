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

collider_t* create_collider(
    double x, double y, double vx, double vy, 
    double w, double h, const char* name, double mass,
    collider_type type, void* target, 
    collider_t* (*apply)(void*, collider_t*, double)
){
    collider_t* collider = (collider_t*)SDL_malloc(sizeof(collider_t));
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
    return collider;
}

void free_collider(collider_t* collider){
    SDL_free(collider);
}

void recalculate_collider_positions(collider_t* collider, double dt){
    collider->x = collider->x + collider->vx * dt;
    collider->y = collider->y + collider->vy * dt;
}

void update_collider(list colliders, double dt){
    double total_dt = 0;
    collider_event_t* winning_event;
    int iterations = 0;
    do{
        //We're gonna check every iterator pair and select the most recent collision
        //We'll apply it and advance everything up to that point
        //And repeat until we have no collisions left in this timeframe.
        SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Collider loop %d: last total_dt = %f of %f", iterations++, total_dt, dt);
        winning_event = NULL;
        list_iterator itera = init_iterator(colliders, 0);
        for (int i = 0; i < length(colliders); i++){
            collider_t* a = (collider_t*)next(itera);
            list_iterator iterb = init_iterator(colliders, i+1);
            while(iterb && has_next(iterb)){
                collider_t* b = (collider_t*)next(iterb);
                collider_event_t* temp = calculate_collision(a, b, dt - total_dt);
                if (temp){
                    if (!winning_event) winning_event = temp;
                    else if (winning_event->dt > temp->dt){
                        SDL_free(winning_event->newa);
                        SDL_free(winning_event->newb);
                        SDL_free(winning_event);
                        winning_event = temp;
                    }
                }
            }
            free_iterator(iterb);
        }
        free_iterator(itera);

        if (winning_event){
            remove(colliders, winning_event->a);
            remove(colliders, winning_event->b);
        }
        list_iterator refreshiter = init_iterator(colliders, 0);
        while(has_next(refreshiter)){
            collider_t* c = (collider_t*)next(refreshiter);
            recalculate_collider_positions(c, winning_event ? winning_event->dt : dt);
            iterator_replace(refreshiter, c->apply(c->target, c, winning_event ? winning_event->dt : dt));
            SDL_free(c);
        }
        if (winning_event){
            SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Collision between %s and %s", winning_event->a->name, winning_event->b->name);
            free_iterator(refreshiter);
            append(colliders, winning_event->newa->apply(winning_event->newa->target, winning_event->newa, winning_event->dt));
            append(colliders, winning_event->newb->apply(winning_event->newb->target, winning_event->newb, winning_event->dt));
            total_dt += winning_event->dt;
            SDL_free(winning_event->a);
            SDL_free(winning_event->b);
            SDL_free(winning_event->newa);
            SDL_free(winning_event->newb);
            SDL_free(winning_event);
        } 
    } while(winning_event);
}

collider_t* collider_copy(collider_t* og){
    collider_t* copy = (collider_t*)SDL_malloc(sizeof(collider_t));
    SDL_memcpy(copy, og, sizeof(collider_t));
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

double calculate_collision_angle(double xa, double ya, double xb, double yb){
    double dx = xa - xb;
    double dy = ya - yb;
    return abs(dx) > 0 ? atan(dy / dx) : ((abs(dy) > 0 ? M_PI * dy / abs(dy) : 0));
}

void quadratic(double a, double b, double c, double* x1, double* x2){
    double disc = SDL_sqrt((b*b)-4*a*c);
    if (disc < 0 || SDL_abs(a) < 0.000001){
        *x1 = NAN;
        *x2 = NAN;
        return;
    } else {
        *x1 = (-b + disc) / (2*a);
        *x2 = (-b - disc) / (2*a);
    }
}

double smallest_positive(double a, double b){
    if (isnan(a)) return (isnan(b) || b < 0) ? NAN : b;
    if (isnan(b)) return (isnan(a) || a < 0) ? NAN : a;
    if (a > 0 && b > 0) return SDL_min(a, b);
    if (a <= 0 && b <= 0) return NAN;
    return SDL_max(a, b);
}

void recalculate_positions(collider_event_t* event){
    event->newa->x = event->a->x + event->a->vx * event->dt;
    event->newa->y = event->a->y + event->a->vy * event->dt;
    event->newb->x = event->b->x + event->b->vx * event->dt;
    event->newb->y = event->b->y + event->b->vy * event->dt;
}

void recalculate_velocities(collider_event_t* event){
    double phi = calculate_collision_angle(event->newa->x, event->newa->y, event->newb->x, event->newb->y);
    double va = sqrt(event->a->vx*event->a->vx + event->a->vy*event->a->vy);
    double vb = sqrt(event->b->vx*event->b->vx + event->b->vy*event->b->vy);
    double thetaa = abs(event->a->vx) > 0 ? atan(-event->a->vy / event->a->vx) : ((abs(event->a->vy) > 0 ? -M_PI * event->a->vy / abs(event->a->vy) : 0));
    double thetab = abs(event->b->vx) > 0 ? atan(-event->b->vy / event->b->vx) : ((abs(event->b->vy) > 0 ? -M_PI * event->b->vy / abs(event->b->vy) : 0));
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "phi %f\ttheta1 %f\ttheta2 %f\tva %f\tvb %f", phi, thetaa, thetab, va, vb);
    // event->newa->vx = (event->a->vx * (event->a->mass - event->b->mass) + event->b->vx * (2 * event->b->mass)) / (event->a->mass + event->b->mass);
    // event->newa->vy = (event->a->vy * (event->a->mass - event->b->mass) + event->b->vy * (2 * event->b->mass)) / (event->a->mass + event->b->mass);
    // event->newb->vx = (event->b->vx * (event->b->mass - event->a->mass) + event->a->vx * (2 * event->a->mass)) / (event->a->mass + event->b->mass);
    // event->newb->vy = (event->b->vy * (event->b->mass - event->a->mass) + event->a->vy * (2 * event->a->mass)) / (event->a->mass + event->b->mass);
    event->newa->vx = cos(phi) * (va * cos(thetaa - phi) * (event->a->mass - event->b->mass) + 2 * event->b->mass * vb * cos(thetab - phi)) / (event->a->mass + event->b->mass) + va * sin(thetaa - phi) * cos(phi + M_PI/2);
    event->newa->vy = sin(phi) * (va * cos(thetaa - phi) * (event->a->mass - event->b->mass) + 2 * event->b->mass * vb * cos(thetab - phi)) / (event->a->mass + event->b->mass) + va * sin(thetaa - phi) * sin(phi + M_PI/2);
    event->newb->vx = cos(phi) * (vb * cos(thetab - phi) * (event->b->mass - event->a->mass) + 2 * event->a->mass * va * cos(thetaa - phi)) / (event->b->mass + event->a->mass) + vb * sin(thetab - phi) * cos(phi + M_PI/2);
    event->newb->vx = sin(phi) * (vb * cos(thetab - phi) * (event->b->mass - event->a->mass) + 2 * event->a->mass * va * cos(thetaa - phi)) / (event->b->mass + event->a->mass) + vb * sin(thetab - phi) * sin(phi + M_PI/2);
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "New velocities: %s (%f, %f)->(%f, %f),\t%s (%f, %f)->(%f, %f)", event->newa->name, event->a->vx, event->a->vy, event->newa->vx, event->newa->vy, event->newb->name, event->b->vx, event->b->vy, event->newb->vx, event->newb->vy);
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
        collider_event_t* event = (collider_event_t*)SDL_malloc(sizeof(collider_event_t));
        event->a = a;
        event->b = b;
        event->dt = dt_ans;
        event->newa = collider_copy(a);
        event->newb = collider_copy(b);
        recalculate_positions(event);
        // double angle = calculate_collision_angle(event->newa->x + a->w/2.0, event->newa->y + a->h/2.0, event->newb->x + b->w/2.0, event->newb->y + b->h/2.0);
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
        bool min_dt_condition = (lt - sqt - sqtlen+ vt*min_dtn - sqvt*min_dtn >= 0 && 
            sqt - lt - ltlen + sqvt*min_dtn - vt*min_dtn >= 0);

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
    collider_event_t* event = (collider_event_t*)SDL_malloc(sizeof(collider_event_t));
    event->a = a;
    event->b = b;
    event->newa = collider_copy(a);
    event->newb = collider_copy(b);
    event->dt = NAN;

    //line a-left on b
    double temp = collide_rect_line(a->x, a->y, a->h, b->x, b->y, b->w, b->h, a->vx, a->vy, b->vx, b->vy);
    if (!isnan(temp) && temp > 0 && (isnan(event->dt) || temp <= event->dt)){
        event->dt = temp;
    }
    //line a-right on b
    temp = collide_rect_line(a->x + a->w, a->y, a->h, b->x, b->y, b->w, b->h, a->vx, a->vy, b->vx, b->vy);
    if (!isnan(temp) && temp > 0 && (isnan(event->dt) || temp <= event->dt)){
        event->dt = temp;
    }
    //line a-top on b
    temp = collide_rect_line(a->y, a->x, a->w, b->y, b->x, b->h, b->w, a->vy, a->vx, b->vy, b->vx);
    if (!isnan(temp) && temp > 0 && (isnan(event->dt) || temp <= event->dt)){
        event->dt = temp;
    }
    //line a-bottom on b
    temp = collide_rect_line(a->y + a->h, a->x, a->w, b->y, b->x, b->h, b->w, a->vy, a->vx, b->vy, b->vx);
    if (!isnan(temp) && temp > 0 && (isnan(event->dt) || temp <= event->dt)){
        event->dt = temp;
    }
    if (!isnan(event->dt) && event->dt > 0 && event->dt <= dt){
        recalculate_positions(event);
        recalculate_velocities(event);
        return event;
    }

    SDL_free(event->newa);
    SDL_free(event->newb);
    SDL_free(event);
    return NULL;
}

double collide_circle_line(double cn, double ct, double r, double vcn, double vct, double ln, double lt, double ll, double vln, double vlt){
    double t1, t2; //temp variables
    double dt_ans;

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
    dt_ans = smallest_positive(t1, t2);

    //Point B
    dn = cn - ln;
    dt = ct - lt - ll;
    dvn = vcn - vln;
    dvt = vct - vlt;

    ca = dvn * dvn + dvt * dvt;
    cb = 2 * (dn * dvn + dt * dvt);
    cc = dn*dn + dt*dt - r*r;
    
    quadratic(ca, cb, cc, &t1, &t2);
    dt_ans = smallest_positive(dt_ans, smallest_positive(t1, t2));

    //Line
    //check if cn in range
    //cn + vcn*t - (ln + vln*t) = R -> cn-ln-R = (vln-vcn)*t -> t = (cn-ln-R)/(vln-vcn)
    double dt_line = smallest_positive(((double)cn-ln-r)/(vln-vcn), ((double)cn-ln+r)/(vln-vcn));
    if (!isnan(dt_line) && dt_line > 0 && (dt_line < dt_ans || isnan(dt_ans))){
        //May be valid so lets check
        if(ct + vct*dt_line >= lt + vlt*dt_line && ct + vct*dt_line <= lt + ll + vlt*dt_line) return dt_line;
        //1585 + 500*0.01 >= 540
    }
    return dt_ans;
}

collider_event_t* collide_circle_rect(collider_t* circle, collider_t* rect, double dt){
    //Calculates a collision between circle and rectangle by dividing rectangle into lines and
    //calculating those collisions. Will fail if circle is fully inside rectangle, but that should
    //never happen and be relevant to detect so we're good i guess.
    //Actually that's a good thing tbh, we can detect inner and outer collisions the exact same way then.

    collider_event_t* event = (collider_event_t*)SDL_malloc(sizeof(collider_event_t));
    event->a = circle;
    event->b = rect;
    event->dt = NAN;
    event->newa = collider_copy(circle);
    event->newb = collider_copy(rect);

    //TOOD Positions for angle stuff.
    //circle on rectangle-left
    double temp = collide_circle_line(circle->x + circle->w/2, circle->y + circle->h/2, circle->h/2, circle->vx, circle->vy, rect->x, rect->y, rect->h, rect->vx, rect->vy);
    if (!isnan(temp) && temp > 0 && (isnan(event->dt) || temp <= event->dt)){
        event->dt = temp;
    }
    //circle on rectangle-right
    temp = collide_circle_line(circle->x + circle->w/2, circle->y + circle->h/2, circle->h/2, circle->vx, circle->vy, rect->x + rect->w, rect->y, rect->h, rect->vx, rect->vy);
    if (!isnan(temp) && temp > 0 && (isnan(event->dt) || temp <= event->dt)){
        event->dt = temp; 
    }
    //circle on rectangle-top
    temp = collide_circle_line(circle->y + circle->h/2, circle->x + circle->w/2, circle->w/2, circle->vy, circle->vx, rect->y, rect->x, rect->w, rect->vy, rect->vx);
    if (!isnan(temp) && temp > 0 && (isnan(event->dt) || temp <= event->dt)){
        event->dt = temp; 
    }
    //circle on rectangle-bottom
    temp = collide_circle_line(circle->y + circle->h/2, circle->x + circle->w/2, circle->w/2, circle->vy, circle->vx, rect->y + rect->h, rect->x, rect->w, rect->vy, rect->vx);
    if (!isnan(temp) && temp > 0 && (isnan(event->dt) || temp <= event->dt)){
        event->dt = temp;
    }

    if (!isnan(event->dt) && event->dt > 0 && event->dt <= dt){
        recalculate_positions(event);
        recalculate_velocities(event);
        SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Updated %s: p(%f, %f), v(%f, %f)", event->newa->name, event->newa->x, event->newa->y, event->newa->vx, event->newa->vy);
        SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Updated %s: p(%f, %f), v(%f, %f)", event->newb->name, event->newb->x, event->newb->y, event->newb->vx, event->newb->vy);
        return event;
    }
    SDL_free(event->newa);
    SDL_free(event->newb);
    SDL_free(event);
    return NULL;
}

// void collider_test_c_c_c(){
//     input_t input;
//     init_text();
//     init_input(&input);
//     collider_t* a = create_collider(0, 0, 0, 0, 200, 200, "a", COLLIDER_CIRCLE, NULL, NULL);
//     collider_t* b = create_collider(200, 200, 0, 0, 200, 200, "b", COLLIDER_CIRCLE, NULL, NULL);
//     //case 1 -> No collision
//     prepareScene(load_texture(ASSET_WALL));
//     presentScene();
//     SDL_Delay(100);

//     collider_event_t* collided = collide_circle_circle(a, b, 1);
//     prepareScene(load_texture(ASSET_WALL));
//     draw_collider_test(a, b, collided, 1);
//     SDL_Texture* text = getTextTexture("1 (nocoll)");
//     SDL_SetTextureColorMod(text, 0, 0, 0);
//     easyblit(text, 0, 800);
//     presentScene();
//     SDL_Delay(2000);

//     //case 2 -> overlap
//     a->x += 40;
//     a->y += 40;
//     b->x -= 40;
//     b->y -= 40;
//     collided = collide_circle_circle(a, b, 1);
//     prepareScene(load_texture(ASSET_WALL));
//     draw_collider_test(a, b, collided, 1);
//     text = getTextTexture("2 (fullcoll)");
//     SDL_SetTextureColorMod(text, 0, 0, 0);
//     easyblit(text, 0, 800);
//     presentScene();
//     SDL_Delay(2000);

//     //case 3 -> graze
//     a->x = 0;
//     a->y = 0;
//     b->x = 141;
//     b->y = 141;
//     collided = collide_circle_circle(a, b, 1);
//     prepareScene(load_texture(ASSET_WALL));
//     draw_collider_test(a, b, collided, 1);
//     text = getTextTexture("3 (graze)");
//     SDL_SetTextureColorMod(text, 0, 0, 0);
//     easyblit(text, 0, 800);
//     presentScene();
//     SDL_Delay(2000);

//     //case 4 -> antigraze
//     a->x = 0;
//     a->y = 0;
//     b->x = 142;
//     b->y = 142;
//     collided = collide_circle_circle(a, b, 1);
//     prepareScene(load_texture(ASSET_WALL));
//     draw_collider_test(a, b, collided, 1);
//     text = getTextTexture("4 (antigraze)");
//     SDL_SetTextureColorMod(text, 0, 0, 0);
//     easyblit(text, 0, 800);
//     presentScene();
//     SDL_Delay(2000);

//     //case 5 -> overlap moving x
//     a->x = 0;
//     a->y = 0;
//     a->vx = 100;
//     a->vy = 0;
//     b->x = 250;
//     b->y = 0;
//     b->vx = 0;
//     b->vy = 0;
//     collided = collide_circle_circle(a, b, 1);
//     prepareScene(load_texture(ASSET_WALL));
//     draw_collider_test(a, b, collided, 1);
//     text = getTextTexture("5 (overlap moving x)");
//     SDL_SetTextureColorMod(text, 0, 0, 0);
//     easyblit(text, 0, 800);
//     presentScene();
//     SDL_Delay(2000);

//     //case 6 -> overlap moving x
//     a->x = 0;
//     a->y = 0;
//     a->vy = 100;
//     a->vx = 0;
//     b->y = 250;
//     b->x = 0;
//     b->vx = 0;
//     b->vy = 0;
//     collided = collide_circle_circle(a, b, 1);
//     prepareScene(load_texture(ASSET_WALL));
//     draw_collider_test(a, b, collided, 1);
//     text = getTextTexture("6 (overlap moving y)");
//     SDL_SetTextureColorMod(text, 0, 0, 0);
//     easyblit(text, 0, 800);
//     presentScene();
//     SDL_Delay(2000);

//     //case 7 -> overlap moving mult
//     a->x = 0;
//     a->y = 0;
//     a->vy = 100;
//     a->vx = 100;
//     b->y = 250;
//     b->x = 250;
//     b->vx = -50;
//     b->vy = -50;
//     collided = collide_circle_circle(a, b, 1);
//     prepareScene(load_texture(ASSET_WALL));    
//     draw_collider_test(a, b, collided, 1);
//     text = getTextTexture("7 (overlap moving)");
//     SDL_SetTextureColorMod(text, 0, 0, 0);
//     easyblit(text, 0, 800);
//     presentScene();
//     SDL_Delay(2000);

//     //case 8 -> ghost
//     a->x = 0;
//     a->y = 0;
//     a->vx = 500;
//     a->vy = 0;
//     b->x = 250;
//     b->y = 0;
//     b->vx = 0;
//     b->vy = 0;
//     collided = collide_circle_circle(a, b, 1);
//     prepareScene(load_texture(ASSET_WALL));
//     draw_collider_test(a, b, collided, 1);
//     text = getTextTexture("8 (ghost-x)");
//     SDL_SetTextureColorMod(text, 0, 0, 0);
//     easyblit(text, 0, 800);
//     presentScene();
//     SDL_Delay(2000);

//     //case 9 -> ghost
//     a->x = 0;
//     a->y = 0;
//     a->vx = 0;
//     a->vy = 500;
//     b->x = 0;
//     b->y = 250;
//     b->vx = 0;
//     b->vy = 0;
//     collided = collide_circle_circle(a, b, 1);
//     prepareScene(load_texture(ASSET_WALL));
//     draw_collider_test(a, b, collided, 1);
//     text = getTextTexture("9 (ghost-y)");
//     SDL_SetTextureColorMod(text, 0, 0, 0);
//     easyblit(text, 0, 800);
//     presentScene();
//     SDL_Delay(2000);

//     //case 10 -> ghost
//     a->x = 0;
//     a->y = 0;
//     a->vx = 500;
//     a->vy = 500;
//     b->x = 200;
//     b->y = 200;
//     b->vx = 50;
//     b->vy = 50;
//     collided = collide_circle_circle(a, b, 1);
//     prepareScene(load_texture(ASSET_WALL));
//     draw_collider_test(a, b, collided, 1);
//     text = getTextTexture("10 (ghost-traverse)");
//     SDL_SetTextureColorMod(text, 0, 0, 0);
//     easyblit(text, 0, 800);
//     presentScene();
//     SDL_Delay(2000);

//     //case 11 -> ghost-traverse overlap
//     a->x = 0;
//     a->y = 0;
//     a->vx = 500;
//     a->vy = 500;
//     b->x = 500;
//     b->y = 500;
//     b->vx = 100;
//     b->vy = 100;
//     collided = collide_circle_circle(a, b, 1);
//     prepareScene(load_texture(ASSET_WALL));
//     draw_collider_test(a, b, collided, 1);
//     text = getTextTexture("11 (ghost-traverse overlap)");
//     SDL_SetTextureColorMod(text, 0, 0, 0);
//     easyblit(text, 0, 800);
//     presentScene();
//     SDL_Delay(2000);

//     //case 12 -> ghost-traverse no overlap
//     a->x = 0;
//     a->y = 0;
//     a->vx = 500;
//     a->vy = 500;
//     b->x = 500;
//     b->y = 500;
//     b->vx = 150;
//     b->vy = 150;
//     collided = collide_circle_circle(a, b, 1);
//     prepareScene(load_texture(ASSET_WALL));
//     draw_collider_test(a, b, collided, 1);
//     text = getTextTexture("12 (ghost-traverse no overlap)");
//     SDL_SetTextureColorMod(text, 0, 0, 0);
//     easyblit(text, 0, 800);
//     presentScene();
//     SDL_Delay(2000);
// }

// void draw_collider_test(collider_t* a, collider_t* b, collider_event_t* event, double dt){
//     SDL_Texture* colla;
//     if (a->type == COLLIDER_CIRCLE) colla = load_texture(ASSET_CIRCLE_DEBUG);
//     else colla = load_texture(ASSET_SQUARE_DEBUG);
//     SDL_Texture* collb;
//     if (b->type == COLLIDER_CIRCLE) collb = load_texture(ASSET_CIRCLE_DEBUG);
//     else collb = load_texture(ASSET_SQUARE_DEBUG);
//     SDL_Texture* namea = getTextTexture((char*)a->name);
//     SDL_Texture* nameb = getTextTexture((char*)b->name);

//     //init
//     SDL_SetTextureColorMod(colla, 0, 0, 0);
//     SDL_SetTextureAlphaMod(colla, 50);
//     SDL_SetTextureColorMod(collb, 0, 0, 0);
//     SDL_SetTextureAlphaMod(collb, 50);
//     SDL_SetTextureColorMod(namea, 0, 0, 0);
//     SDL_SetTextureAlphaMod(namea, 50);
//     SDL_SetTextureColorMod(nameb, 0, 0, 0);
//     SDL_SetTextureAlphaMod(nameb, 50);
//     blit(colla, {(int)a->x, (int)a->y, (int)a->w, (int)a->h}, 0);
//     blit(collb, {(int)b->x, (int)b->y, (int)b->w, (int)b->h}, 0);
//     blit(namea, {(int)(a->x + a->w/4), (int)(a->y + a->h/4), (int)a->w/2, (int)a->h/2}, 0);
//     blit(nameb, {(int)(b->x + b->w/4), (int)(b->y + b->h/4), (int)b->w/2, (int)b->h/2}, 0);
//     //end
//     blit(colla, {(int)(a->x + a->vx*dt), (int)(a->y + a->vy*dt), (int)a->w, (int)a->h}, 0);  
//     blit(colla, {(int)(b->x + b->vx*dt), (int)(b->y + b->vy*dt), (int)a->w, (int)a->h}, 0);  
//     blit(namea, {(int)(a->x + a->vx*dt + a->w/4), (int)(a->y + a->vy*dt + a->h/4), (int)a->w/2, (int)a->h/2}, 0);
//     blit(nameb, {(int)(b->x + b->vx*dt + b->w/4), (int)(b->y + b->vy*dt + b->h/4), (int)b->w/2, (int)b->h/2}, 0);

//     if (event){
//         //Collision
//         SDL_SetTextureColorMod(colla, 255, 0, 0);
//         SDL_SetTextureColorMod(collb, 255, 0, 0);
//         SDL_SetTextureColorMod(namea, 255, 0, 0);
//         SDL_SetTextureColorMod(nameb, 255, 0, 0);
//         blit(colla, {(int)event->newa->x, (int)event->newa->y, (int)event->newa->w, (int)event->newa->h}, 0);
//         blit(collb, {(int)event->newb->x, (int)event->newb->y, (int)event->newb->w, (int)event->newb->h}, 0);
//         blit(namea, {(int)(event->newa->x + event->newa->w/4), (int)(event->newa->y + event->newa->h/4), (int)event->newa->w/2, (int)event->newa->h/2}, 0);
//         blit(nameb, {(int)(event->newb->x + event->newb->w/4), (int)(event->newb->y + event->newb->h/4), (int)event->newb->w/2, (int)event->newb->h/2}, 0);

//         //Finalp
//         SDL_SetTextureColorMod(colla, 0, 255, 0);
//         SDL_SetTextureColorMod(collb, 0, 255, 0);
//         SDL_SetTextureColorMod(namea, 0, 255, 0);
//         SDL_SetTextureColorMod(nameb, 0, 255, 0);
//         SDL_SetTextureAlphaMod(colla, 255);
//         SDL_SetTextureAlphaMod(collb, 255);
//         SDL_SetTextureAlphaMod(namea, 255);
//         SDL_SetTextureAlphaMod(namea, 255);

//         blit(colla, {(int)(event->newa->x + event->newa->vx*(dt - event->dt)), (int)(event->newa->y + event->newa->vy*(dt - event->dt)), (int)event->newa->w, (int)event->newa->h}, 0);  
//         blit(colla, {(int)(event->newb->x + event->newb->vx*(dt - event->dt)), (int)(event->newb->y + event->newb->vy*(dt - event->dt)), (int)event->newa->w, (int)event->newa->h}, 0);  
//         blit(namea, {(int)(event->newa->x + event->newa->vx*(dt - event->dt) + event->newa->w/4), (int)(event->newa->y + event->newa->vy*(dt-event->dt) + event->newa->h/4), (int)event->newa->w/2, (int)event->newa->h/2}, 0);
//         blit(nameb, {(int)(event->newb->x + event->newb->vx*(dt - event->dt) + event->newb->w/4), (int)(event->newb->y + event->newb->vy*(dt-event->dt) + event->newb->h/4), (int)event->newb->w/2, (int)event->newb->h/2}, 0);
//     }
    
// }