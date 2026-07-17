#ifndef __CHARACTER_SHOW_H
#define __CHARACTER_SHOW_H

#define CHARACTER_CLUETIP "Esc return to selector"

#define STAT_WIDTH 160
#define STAT_HEIGHT 120

#define STAT_MOD_HEIGHT 60

#define STAT_NAME_HEIGHT 30

#define NAME_X 200
#define NAME_Y 100

#define NAME_WIDTH 1200
#define NAME_HEIGHT 120

#define JOB_WIDTH 1500
#define JOB_HEIGHT 80

#define JOB_X 200
#define JOB_Y 250

#define STR_X 200
#define STR_Y 385
#define STR_NAME "Strength"

#define DEX_X 400
#define DEX_Y STR_Y
#define DEX_NAME "Dexterity"

#define CON_X 600
#define CON_Y STR_Y
#define CON_NAME "Constitution"

#define INT_X STR_X
#define INT_Y 685
#define INT_NAME "Intelligence"

#define WIS_Y INT_Y
#define WIS_X DEX_X
#define WIS_NAME "Wisdom"

#define CHA_X CON_X
#define CHA_Y INT_Y
#define CHA_NAME "Charisma~"

void show_character(character_t* character);

#endif