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

void display_character_select_screen(vector<character_t*> characters, size_t hovering_selected){
    prepareScene(selector_bg);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    for (size_t i = 0; i < characters.size(); i++){
        display_text_border(font, characters[i]->name, XPOS, YPOS + i * ITEM_HEIGHT, TEXT_HEIGHT, MAX_X, i == hovering_selected);
    }

    display_text_border(font, SELECTOR_CLUETIP, CLUETIP_X, CLUETIP_Y, CLUETIP_H, CLUETIP_W, false);
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