// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
using namespace std;

#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <defs.h>
#include <draw.h>

extern SDL_Renderer* renderer;

void prepareScene(SDL_Texture* bg){
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    blit(bg, {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT}, 0);
}

void presentScene(){
    SDL_RenderPresent(renderer);
}

SDL_Texture *load_texture(char* filename){
    SDL_Texture *texture;
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", filename);
    texture = IMG_LoadTexture(renderer, filename);
    return texture;
}

void easyblit(SDL_Texture* texture, int x, int y){
    SDL_Rect dest;

    dest.x = x;
    dest.y = y;
    SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);

    blit(texture, dest, 0);
}

void blit(SDL_Texture *texture, SDL_Rect dstRect, float angle){
    SDL_RenderCopyEx(renderer, texture, NULL, &dstRect, angle, NULL, SDL_FLIP_NONE);
}