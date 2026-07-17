
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
extern SDL_Texture* border;

void display_stat(string text, string mod, int x, int y, int h, int maxw, int hmod){
    SDL_Rect border_transform = {x, y, 170, 200};
    blit(border, border_transform, 0);

    SDL_Rect stat_transform;
    SDL_Texture* stat_texture = text_crop_to_fit_bounds(font, stat_transform, text, h, maxw);
    
    stat_transform.y = y + 20;
    stat_transform.x = x + border_transform.w/2 - stat_transform.w/2;
    
    blit(stat_texture, stat_transform, 0);
    SDL_DestroyTexture(stat_texture);

    if (!mod.empty()){
        SDL_Rect stat_mod_transform;
        SDL_Texture* stat_mod_texture = text_crop_to_fit_bounds(font, stat_mod_transform, mod, hmod, maxw);
        stat_mod_transform.y = y + stat_transform.h + 15;
        stat_mod_transform.x = x + border_transform.w/2 - stat_mod_transform.w/2;
        blit(stat_mod_texture, stat_mod_transform, 0);
        SDL_DestroyTexture(stat_mod_texture);
    }
    
}

string mod_to_string(int mod){
    if (mod > 0) 
        return "+" + to_string(mod);
    else  
        return to_string(mod);
}

void display_character_stats(character_t* character){
    display_text_border(font, character->name, NAME_X, NAME_Y, NAME_HEIGHT, NAME_WIDTH, false);
    display_text_border(font, character->job, JOB_X, JOB_Y, JOB_HEIGHT, JOB_WIDTH, false);
    display_stat(to_string(character->strength), mod_to_string(character->strengthMod), STR_X, STR_Y, STAT_HEIGHT, STAT_WIDTH, STAT_MOD_HEIGHT);
    display_stat(to_string(character->dexterity), mod_to_string(character->dexterityMod), DEX_X, DEX_Y, STAT_HEIGHT, STAT_WIDTH, STAT_MOD_HEIGHT);
    display_stat(to_string(character->constitution), mod_to_string(character->constitutionMod), CON_X, CON_Y, STAT_HEIGHT, STAT_WIDTH, STAT_MOD_HEIGHT);
    display_stat(to_string(character->intelligence), mod_to_string(character->intelligenceMod), INT_X, INT_Y, STAT_HEIGHT, STAT_WIDTH, STAT_MOD_HEIGHT);
    display_stat(to_string(character->wisdom), mod_to_string(character->wisdomMod), WIS_X, WIS_Y, STAT_HEIGHT, STAT_WIDTH, STAT_MOD_HEIGHT);
    display_stat(to_string(character->charisma), mod_to_string(character->charismaMod), CHA_X, CHA_Y, STAT_HEIGHT, STAT_WIDTH, STAT_MOD_HEIGHT);
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
    display_text_border(font, CHARACTER_CLUETIP, 200, 1000, 40, 1500, false);
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