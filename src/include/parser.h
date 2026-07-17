#ifndef __PARSER_H
#define __PARSER_H
using namespace std;
#include <vector>
#include <character.h>

int parse(vector<character_t*>& characters);
void dump(vector<character_t*> characters);
void clean_characters(vector<character_t*>& characters);
void parse_lyric(vector<timedstring*>& vector);
void destroy_lyrics(vector<timedstring*>& vector);
#endif