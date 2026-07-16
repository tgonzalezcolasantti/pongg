#ifndef __CHARACTER_H
#define __CHARACTER_H
using namespace std;
#include <string>
#include <stdbool.h>
#include <stdint.h>

typedef struct character_t
{
    char* name;
    char* job;
    int strength;
    int dexterity;
    int constitucion;
    int intelligence;
    int wisdom;
    int charisma;
} character_t;





#endif