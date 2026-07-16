 
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <character.h>
#include <cstdlib>

using namespace std;
 
int bullshit(vector<character_t*> &characters)
{
	ifstream file("character.txt");
	string line;
    
	cout << "Personajes de la campaña:" << endl;
	getline(file, line); //ignore header
    while (getline(file, line))
    {
        stringstream linestream(line);
        string attribute;
        character_t* character = new character_t;
        for (size_t i = 0; i < 8; i++)
        {
            getline(linestream, attribute, ';');
            
            switch (i)
            {
                case (0):
                    character->name = attribute;
                    break;
                case (1):
                    character->job = attribute;
                    break;
                case (2):
                    character->strength = stoi(attribute, NULL, 10);
                    break;
                case (3):
                    character->dexterity = stoi(attribute, NULL, 10);
                    break;
                case (4):
                    character->constitucion = stoi(attribute, NULL, 10);
                    break;
                case (5):
                    character->intelligence = stoi(attribute, NULL, 10);
                    break;
                case (6):
                    character->wisdom = stoi(attribute, NULL, 10);
                    break;
                case (7):
                    character->charisma = stoi(attribute, NULL, 10);
                    break;
                default:
                    break;
            }
	    }
        characters.push_back(character);
    }
	return characters.size();
}
 