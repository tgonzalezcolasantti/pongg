#ifndef __TEXT_H
#define __TEXT_H

#include <SDL.h>
#include <SDL_ttf.h>

SDL_Texture* getTextTexture(char* text, TTF_Font* font, SDL_Renderer* renderer);

#endif