#ifndef __TEXT_H
#define __TEXT_H

#include <SDL.h>
#include <SDL_ttf.h>

SDL_Texture* getTextTexture(char* text);
void init_text();

#endif