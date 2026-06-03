#ifndef __DRAW_H
#define __DRAW_H

#include <SDL.h>
#include <defs.h>

void prepareScene(SDL_Renderer* renderer);

void presentScene(SDL_Renderer* renderer);

SDL_Texture *load_texture(SDL_Renderer* renderer, char* filename);

void draw_entity(SDL_Renderer* renderer, Entity* entity);
void blit(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y);

#endif