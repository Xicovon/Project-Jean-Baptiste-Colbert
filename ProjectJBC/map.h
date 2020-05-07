#pragma once
#include <string>
#include <vector>
#include "province.h"
#include "provinceTerrain.h"
#include <fstream>
using namespace std;

class Map {
private:
	string name;
	vector<Province*> provinces;
	vector<ProvinceTerrain*> terrain_types;
	//shape (rectangle/sphere)
public:
	Map() {
		name = "default";
	}
	//go to folder and open necessary files to intialize provinces
	Map(string map_path) {
		ifstream input_file;
		input_file.open(map_path + "terrain_types.csv");

		if (!input_file) {
			cerr << "Unable to open file " << map_path << "terrain_types.csv";
			exit(1);   // call system to stop
		}

		int line_number = 0;
		string line;
		while (getline(input_file, line)) {
			if (++line_number != 1) {
				//if the line is not the header, then create a new terrain object
				int id = -1;
				string name = "Default Terrain";
				double movement_speed = 1;

				int index = line.find(",", 0);

				//get id column
				id = stoi(line.substr(0, index));
				line = line.substr(index + 1, line.length());

				//get name column
				index = line.find(",", 0);
				name = line.substr(0, index);

				//get movement speed column
				line = line.substr(index + 1, line.length());
				movement_speed =  stod(line);

				//create new terrain object to add to list at id index
				ProvinceTerrain* new_terrain = new ProvinceTerrain(name, movement_speed);
				terrain_types.insert(terrain_types.begin() + id, new_terrain);
			}
		}

		input_file.close();
	}
};