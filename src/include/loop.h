#ifndef __LOOP_H
#define __LOOP_H

#include <SDL.h>
#include <input.h>

void run_game_loop();
bool run_frame(uint64_t lastTicks, input_t input);

#endif