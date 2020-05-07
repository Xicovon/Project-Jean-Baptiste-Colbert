#pragma once
#include <string>
using namespace std;

class ProvinceTerrain {
private:
	string display_name;
	double movement_speed;

public:
	ProvinceTerrain() {
		display_name = "Default Terrain";
		movement_speed = 1;
	}
	ProvinceTerrain(string init_display_name, double init_movement_speed) {
		display_name = init_display_name;
		movement_speed = init_movement_speed;
	}
};