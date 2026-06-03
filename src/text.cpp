// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
using namespace std;

#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#include <defs.h>
#include <loop.h>
#include <draw.h>

SDL_Texture* toTexture(SDL_Surface *surface, bool destroySurface, SDL_Renderer* renderer){
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (destroySurface)
        SDL_FreeSurface(surface);

    return texture;
}

SDL_Texture* getTextTexture(char* text, TTF_Font* font, SDL_Renderer* renderer){
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text, {255, 255, 255, 0});

    return toTexture(surface, true, renderer);
}