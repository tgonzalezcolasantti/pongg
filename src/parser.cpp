 
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <character.h>
#include <cstdlib>

using namespace std;
string header = "nombre;clase;fuerza;fuerzaMod;destreza;destrezaMod;constitucion;constitucionMod;inteligencia;inteligenciaMod;sabiduria;sabiduriaMod;carisma;carismaMod";

 
int parse(vector<character_t*> &characters)
{
	ifstream file("character.txt");
	string line;
    
	getline(file, line); //ignore header
    while (getline(file, line))
    {
        stringstream linestream(line);
        string attribute;
        character_t* character = new character_t;
        for (size_t i = 0; i < TOTAL_ATTRIBUTES; i++)
        {
            getline(linestream, attribute, ';');
            
            switch (i)
            {
                case (NAME):
                    character->name = attribute;
                    break;
                case (JOB):
                    character->job = attribute;
                    break;
                case (STRENGTH):
                    character->strength = stoi(attribute, NULL, 10);
                    break;
                case (STRENGTHMOD):
                    character->strengthMod = stoi(attribute, NULL, 10);
                    break;
                case (DEXTERITY):
                    character->dexterity = stoi(attribute, NULL, 10);
                    break;
                case (DEXTERITYMOD):
                    character->dexterityMod = stoi(attribute, NULL, 10);
                    break;                
                case (CONSTITUTION):
                    character->constitution = stoi(attribute, NULL, 10);
                    break;
                case (CONSTITUTIONMOD):
                    character->constitutionMod = stoi(attribute, NULL, 10);
                    break;
                case (INTELLIGENCE):
                    character->intelligence = stoi(attribute, NULL, 10);
                    break;
                case (INTELLIGENCEMOD):
                    character->intelligenceMod = stoi(attribute, NULL, 10);
                    break;
                case (WISDOM):
                    character->wisdom = stoi(attribute, NULL, 10);
                    break;
                case (WISDOMMOD):
                    character->wisdomMod = stoi(attribute, NULL, 10);
                    break;
                case (CHARISMA):
                    character->charisma = stoi(attribute, NULL, 10);
                    break;
                case (CHARISMAMOD):
                    character->charismaMod = stoi(attribute, NULL, 10);
                    break;
                case (IMAGE):
                    character->image = attribute;
                    break;
                default:
                    break;
            }
	    }
        characters.push_back(character);
    }
	return characters.size();
}

void dump(vector<character_t*> characters){
    ofstream file("character.txt", ios_base::trunc);
    file << header;

    for(size_t i = 0; i < characters.size(); i++){
        character_t* character = characters[i];
        file << "\n";
        file << character->name << ";";
        file << character->job << ";";
        file << character->strength << ";";
        file << character->strengthMod << ";";
        file << character->dexterity << ";";
        file << character->dexterityMod << ";";
        file << character->constitution << ";";
        file << character->constitutionMod << ";";
        file << character->intelligence << ";";
        file << character->intelligenceMod << ";";
        file << character->wisdom << ";";
        file << character->wisdomMod << ";";
        file << character->charisma << ";";
        file << character->charismaMod << ";";
        file << character->image;
    }
}
 