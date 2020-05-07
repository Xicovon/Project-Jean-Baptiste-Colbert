#pragma once
#include <string>
#include "provinceTerrain.h"
#include <vector>
using namespace std;

class Province {
private:
	string name;
	ProvinceTerrain* terrain_type;
	vector<Province*> adjacent_provinces; //list of pointers to connected provinces

	//mesh of vertices/shape
	//owner: reference to occupying faction
	//economic factors
	//list of references to population units
	//list of references to resource objects and their value as %
	//list of improvements: buildings/roads

public:
	Province() {
		name = "Default Province";
	}
	Province(string init_province_name) {
		name = init_province_name;
	}
};