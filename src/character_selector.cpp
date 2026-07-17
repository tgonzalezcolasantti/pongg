#include <vector>
#include <character.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <input.h>
#include <draw.h>
#include <character_selector.h>
#include <text.h>
#include <iostream>
using namespace std;

extern SDL_Texture* selector_bg;
extern TTF_Font* font;
extern SDL_Renderer* renderer; 
extern input_t input;

void display_character(string name, int index, bool selected){
    SDL_Texture* name_texture = NULL;
    SDL_Rect transform;
    name_texture = text_crop_to_fit_bounds(font, transform, name, TEXT_HEIGHT, MAX_X);

    transform.y = YPOS + ITEM_HEIGHT * index;
    transform.x = XPOS;

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

void display_character_select_screen(vector<character_t*> characters, size_t hovering_selected){
    prepareScene(selector_bg);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    for (size_t i = 0; i < characters.size(); i++){
        display_character(characters[i]->name, i, i == hovering_selected);
    }
    SDL_Rect transform;
    SDL_Texture* cluetip = text_crop_to_fit_bounds(font, transform, CLUETIP, 40, 1500);

    transform.y = 1000;
    transform.x = 200;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 100);
    SDL_RenderFillRect(renderer, &transform);

    blit(cluetip, transform, 0);
    SDL_DestroyTexture(cluetip);
    presentScene();
}

/*
Receives a list of characters and displays a selector screen. 
Returns the index of the selected character
*/
ssize_t select_character(vector<character_t*> characters){
    size_t hovering_selected = 0;
    cout << characters.size() << endl;
    while(true){
        display_character_select_screen(characters, hovering_selected);
        handle_input(&input, true);
        if (input.down && hovering_selected < characters.size() - 1){
            hovering_selected++;
        } else if (input.up && hovering_selected > 0){
            hovering_selected--;
        } else if (input.select){
            return hovering_selected;
        } else if (input.back){
            return -1;
        }
    }
} 