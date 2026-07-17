using namespace std;

#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#include <defs.h>
#include <draw.h>

extern SDL_Renderer* renderer;
string lyric;
extern TTF_Font* font;

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

void display_text_border(TTF_Font* font, string name, int x, int y, int h, int maxw, bool selected){
    SDL_Texture* name_texture = NULL;
    SDL_Rect transform;
    name_texture = text_crop_to_fit_bounds(font, transform, name, h, maxw);

    transform.y = y;
    transform.x = x;

    if (selected){
        SDL_SetRenderDrawColor(renderer, 127, 0, 0, 200);
        SDL_RenderFillRect(renderer, &transform);
    } else {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
        SDL_RenderFillRect(renderer, &transform);
    }

    blit(name_texture, transform, 0);
    SDL_DestroyTexture(name_texture);
}

void set_lyrics(string newlyric){
    lyric = newlyric;
}

void show_lyrics(){
    if (!lyric.empty()){
        display_text_border(font, lyric, 1200, 1000, 40, 800, false);
    }
}