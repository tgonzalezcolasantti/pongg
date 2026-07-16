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
    bool fits = false;
    SDL_Texture* name_texture;
    SDL_Rect transform;
    do{
        name_texture = getTextTexture(font, name);
        SDL_QueryTexture(name_texture, NULL, NULL, &transform.w, &transform.h);
        if (transform.h != TEXT_HEIGHT){
            double resize_ratio = (double)TEXT_HEIGHT / transform.h;
            transform.h *= resize_ratio;
            transform.w *= resize_ratio;
        }
        if (transform.w > MAX_X){
            name = name.substr(0, name.length() - 4).append("...");
        } else 
            fits=true;
    } while(!fits);

    transform.y = YPOS + ITEM_HEIGHT * index;
    transform.x = XPOS;

    if (selected){
        SDL_SetRenderDrawColor(renderer, 127, 0, 0, 50);
        SDL_RenderDrawRect(renderer, &transform);
    }

    blit(name_texture, transform, 0);
    SDL_DestroyTexture(name_texture);
}

void display_character_select_screen(vector<character_t> characters, size_t hovering_selected){
    prepareScene(selector_bg);
    for (int i = 0; i < characters.size(); i++){
        display_character(characters[i].name, i, i == hovering_selected);
    }
    presentScene();
}

/*
Receives a list of characters and displays a selector screen. 
Returns the index of the selected character
*/
ssize_t select_character(vector<character_t> characters){
    int hovering_selected = 0;

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