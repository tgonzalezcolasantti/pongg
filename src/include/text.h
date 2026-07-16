#ifndef __TEXT_H
#define __TEXT_H

#include <SDL.h>
#include <SDL_ttf.h>

SDL_Texture* getTextTexture(TTF_Font* font, string text);

#endif