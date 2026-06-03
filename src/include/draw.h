#ifndef __DRAW_H
#define __DRAW_H

#include <SDL.h>
#include <defs.h>

void prepareScene(SDL_Texture* bg);

void presentScene();

SDL_Texture* load_texture(char* filename);

void easyblit(SDL_Texture* texture, int x, int y);
void blit(SDL_Texture *texture, SDL_Rect dstRect, float angle);
#endif