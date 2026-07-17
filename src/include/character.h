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
    int strengthMod;
    int dexterity;
    int dexterityMod;
    int constitution;
    int constitutionMod;
    int intelligence;
    int intelligenceMod;
    int wisdom;
    int wisdomMod;
    int charisma;
    int charismaMod;
    string image;
} character_t;

typedef enum character_attribute{
    NAME = 0,
    JOB,
    STRENGTH,
    STRENGTHMOD,
    DEXTERITY,
    DEXTERITYMOD,
    CONSTITUTION,
    CONSTITUTIONMOD,
    INTELLIGENCE,
    INTELLIGENCEMOD,
    WISDOM,
    WISDOMMOD,
    CHARISMA,
    CHARISMAMOD,
    IMAGE,
    TOTAL_ATTRIBUTES
}character_attribute;




#endif