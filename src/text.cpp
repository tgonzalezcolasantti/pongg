// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
using namespace std;

#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#include <defs.h>
#include <draw.h>

extern SDL_Renderer* renderer;

SDL_Texture* toTexture(SDL_Surface *surface, bool destroySurface){
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (destroySurface)
        SDL_FreeSurface(surface);

    return texture;
}

SDL_Texture* getTextTexture(TTF_Font* font, string text){
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text.c_str(), {255, 255, 255, 0});

    return toTexture(surface, true);
}

SDL_Rect fit_text_to_height(SDL_Texture* text, int height){
    SDL_Rect transform;
    SDL_QueryTexture(text, NULL, NULL, &transform.w, &transform.h);
    if (transform.h != height){
        double resize_ratio = (double)height / transform.h;
        transform.h *= resize_ratio;
        transform.w *= resize_ratio;
    }
    return transform;
}

SDL_Texture* text_crop_to_fit_bounds(TTF_Font* font, SDL_Rect& transform, string text, int height, int max_width){
    SDL_Texture* text_texture = NULL;
    bool fits = false;
    do{
        if (text_texture) 
            SDL_DestroyTexture(text_texture);
            
        text_texture = getTextTexture(font, text);
        transform = fit_text_to_height(text_texture, height);
        if (transform.w > max_width){
            text = text.substr(0, text.length() - 4).append("...");
        } else 
            fits=true;
    } while(!fits);
    return text_texture;
}