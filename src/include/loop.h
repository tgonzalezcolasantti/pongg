#ifndef __LOOP_H
#define __LOOP_H

#include <SDL.h>
#include <input.h>

void run_game_loop(SDL_Renderer* renderer);
void run_frame(SDL_Renderer* renderer, uint64_t lastTicks, input_t input);

#endif