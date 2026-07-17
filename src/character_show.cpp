
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

void display_stat(string text, string mod, string name, int x, int y){
    SDL_Rect border_transform = {x, y, BORDER_WIDTH, BORDER_HEIGHT};
    blit(border, border_transform, 0);

    SDL_Rect stat_transform;
    SDL_Texture* stat_texture = text_crop_to_fit_bounds(font, stat_transform, text, STAT_HEIGHT, STAT_WIDTH);
    
    stat_transform.y = y + STAT_Y_OFFSET;
    stat_transform.x = x + border_transform.w/2 - stat_transform.w/2;
    
    blit(stat_texture, stat_transform, 0);
    SDL_DestroyTexture(stat_texture);

    SDL_Rect stat_mod_transform;
    SDL_Texture* stat_mod_texture = text_crop_to_fit_bounds(font, stat_mod_transform, mod, STAT_MOD_HEIGHT, STAT_WIDTH);
    stat_mod_transform.y = y + stat_transform.h + STAT_MOD_Y_OFFSET;
    stat_mod_transform.x = x + border_transform.w/2 - stat_mod_transform.w/2;
    blit(stat_mod_texture, stat_mod_transform, 0);
    SDL_DestroyTexture(stat_mod_texture);

    SDL_Rect stat_name_transform;
    SDL_Texture* stat_name_texture = text_crop_to_fit_bounds(font, stat_name_transform, name, STAT_NAME_HEIGHT, STAT_WIDTH);
    stat_name_transform.y = stat_transform.y + STAT_NAME_Y_OFFSET;
    stat_name_transform.x = x + border_transform.w/2 - stat_name_transform.w/2;
    blit(stat_name_texture, stat_name_transform, 0);
    SDL_DestroyTexture(stat_name_texture);
    
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
    display_stat(to_string(character->strength), mod_to_string(character->strengthMod), STR_NAME, STR_X, STR_Y);
    display_stat(to_string(character->dexterity), mod_to_string(character->dexterityMod), DEX_NAME, DEX_X, DEX_Y);
    display_stat(to_string(character->constitution), mod_to_string(character->constitutionMod), CON_NAME, CON_X, CON_Y);
    display_stat(to_string(character->intelligence), mod_to_string(character->intelligenceMod), INT_NAME, INT_X, INT_Y);
    display_stat(to_string(character->wisdom), mod_to_string(character->wisdomMod), WIS_NAME, WIS_X, WIS_Y);
    display_stat(to_string(character->charisma), mod_to_string(character->charismaMod), CHA_NAME, CHA_X, CHA_Y);
}

void display_image(SDL_Texture* image){
    SDL_Rect transform = {IMAGE_X, IMAGE_Y, IMAGE_W, IMAGE_H};
    blit(image, transform, 0);
}


void display_character_screen(character_t* character, SDL_Texture* image){
    prepareScene(selector_bg);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    display_character_stats(character);
    display_image(image);
    display_text_border(font, CHARACTER_CLUETIP, CLUETIP_X, CLUETIP_Y, CLUETIP_H, CLUETIP_W, false);
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