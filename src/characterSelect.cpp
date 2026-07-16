 
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
using namespace std;
 
int bullshit()
{
	int personajeSelect;
	vector <string> tremendos10;
	ifstream file("tremendos10.txt");
	string line;
	cout << "Personajes de la campaña:" << endl;
 
	while (getline(file, line)) {
 
		cout << line << endl;
 
		tremendos10.push_back(line);
	}
 
	cout << "jugadores activos: " << endl;
	for (int i = 0; i < tremendos10.size(); i++){
		cout << tremendos10[i] << endl;
 
	}
	cout << "elegi a un Personaje: " <<endl;
	cin >> personajeSelect;
	cout << "selecionaste:" << tremendos10[personajeSelect] << endl;
}
 