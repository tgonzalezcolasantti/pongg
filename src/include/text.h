#ifndef __TEXT_H
#define __TEXT_H

#include <SDL.h>
#include <SDL_ttf.h>

#define CLUETIP_X 200
#define CLUETIP_Y 1000
#define CLUETIP_W 40
#define CLUETIP_H 1500

SDL_Texture* getTextTexture(TTF_Font* font, string text);
SDL_Rect fit_text_to_height(SDL_Texture* text, int height);
SDL_Texture* text_crop_to_fit_bounds(TTF_Font* font, SDL_Rect& transform, string text, int height, int max_width);
void display_text_border(TTF_Font* font, string name, int x, int y, int h, int maxw, bool selected);

#endif