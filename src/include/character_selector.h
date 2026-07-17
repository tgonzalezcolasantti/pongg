#ifndef __CHARACTER_SELECTOR
#define __CHARACTER_SELECTOR
using namespace std;
#include <vector>
#include <character.h>

#define SELECTOR_CLUETIP "Up/Down arrows to move list    Enter select    Esc exit"

#define TOTAL_SELECTOR_ELEMENTS 5ul
#define XPOS 200
#define YPOS 100
#define ITEM_HEIGHT 100
#define TEXT_HEIGHT 66
#define MAX_X 1500

unsigned long select_character(vector<character_t*> characters);

#endif