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
	glm::vec3 rgb;
	vector<glm::vec3> vertices;

	//mesh of vertices/shape
	//owner: reference to occupying faction
	//economic factors
	//list of references to population units
	//list of references to resource objects and their value as %
	//list of improvements: buildings/roads

public:
	//constructors
	Province() {
		name = "Default Province";
		rgb = glm::vec3(0, 0, 0);
	}
	Province(string init_province_name, glm::vec3 init_rgb, ProvinceTerrain* init_terrain_type) {
		name = init_province_name;
		rgb = init_rgb;
		terrain_type = init_terrain_type;
	}

	//accessors
	string GetName() {
		return name;
	}
	ProvinceTerrain* GetProvinceTerrain() {
		return terrain_type;
	}
	vector<Province*> GetAdjacentProvinces() {
		return adjacent_provinces;
	}
	glm::vec3 GetRGB() {
		return rgb;
	}
	vector<glm::vec3>* GetVertices() {
		return &vertices;
	}
};