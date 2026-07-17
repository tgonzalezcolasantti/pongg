#ifndef __CHARACTER_H
#define __CHARACTER_H
using namespace std;
#include <string>
#include <stdbool.h>
#include <stdint.h>

typedef struct character_t
{
    string name;
    string job;
    int strength;
    int strength_mod;
    int dexterity;
    int dexterity_mod;
    int constitucion;
    int constitucion_mod;
    int intelligence;
    int intelligence_mod;
    int wisdom;
    int wisdom_mod;
    int charisma;
    int charisma_mod;
} character_t;





#endif