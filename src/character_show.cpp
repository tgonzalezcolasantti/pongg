
#include <vector>
#include <character.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <input.h>
#include <draw.h>
#include <character_selector.h>
#include <text.h>
#include <iostream>
#include <character_show.h>
#include <SDL_image.h>
using namespace std;

extern SDL_Texture* selector_bg;
extern TTF_Font* font;
extern SDL_Renderer* renderer; 
extern input_t input;

void display_stat(string text,int x,int y,int h, int maxw){
    SDL_Texture* job_texture = NULL;
    SDL_Rect transform;
    job_texture = text_crop_to_fit_bounds(font, transform, text , h, maxw);

    transform.y = y;
    transform.x = x;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 100);
    SDL_RenderFillRect(renderer, &transform);

    blit(job_texture, transform, 0);
    SDL_DestroyTexture(job_texture);
}

void display_character_stats(character_t* character){
    display_stat(character->name,NAME_X,NAME_Y,NAME_HEIGHT,NAME_WIDTH);
    display_stat(character->job,JOB_X,JOB_Y,JOB_HEIGHT,JOB_WIDTH);
    display_stat(to_string(character->strength),STR_X,STR_Y,STAT_HEIGHT,STAT_WIDTH);
    display_stat(to_string(character->dexterity),DEX_X,DEX_Y,STAT_HEIGHT,STAT_WIDTH);
    display_stat(to_string(character->constitution),CON_X,CON_Y,STAT_HEIGHT,STAT_WIDTH);
    display_stat(to_string(character->intelligence),INT_X,INT_Y,STAT_HEIGHT,STAT_WIDTH);
    display_stat(to_string(character->wisdom),WIS_X,WIS_Y,STAT_HEIGHT,STAT_WIDTH);
    display_stat(to_string(character->charisma),CHA_X,CHA_Y,STAT_HEIGHT,STAT_WIDTH);
}

void display_image(SDL_Texture* image){
    SDL_Rect transform = {1230,75,620,930};
    blit(image, transform, 0);
}


void display_character_screen(character_t* character, SDL_Texture* image){
    prepareScene(selector_bg);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    display_character_stats(character);
    display_image(image);
    display_stat(CHARACTER_CLUETIP, 200, 1000, 40, 1500);
    presentScene();
}

void show_character(character_t* character){
    SDL_Texture* image = IMG_LoadTexture(renderer, character->image.c_str());
    while(true){
        display_character_screen(character, image);
        handle_input(&input, true);
        if (input.back){
            break;
        }
    }
    SDL_DestroyTexture(image);
} 