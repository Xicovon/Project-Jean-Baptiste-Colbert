#pragma once
#include <string>
#include <vector>
#include "province.h"
#include "provinceTerrain.h"
#include <fstream>
#include <algorithm>
using namespace std;

extern "C" {
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
}

class Map {
private:
	string name;
	vector<Province*> provinces;
	vector<ProvinceTerrain*> terrain_types;
	//shape (rectangle/sphere)

	//function loads csv from file into list of terrain types
	void LoadProvinceTerrain(string file_path) {
		ifstream input_file;
		input_file.open(file_path);

		if (!input_file) {
			cerr << "Unable to open file " << file_path;
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
				movement_speed = stod(line);

				//create new terrain object to add to list at id index
				ProvinceTerrain* new_terrain = new ProvinceTerrain(name, movement_speed);
				//make sure vector has enough indexes
				terrain_types.resize(max(id + 1, (int)terrain_types.size()));
				//insert province to correct index
				terrain_types.at(id) = new_terrain;
			}
		}
		input_file.close();
	}

	//function loads provinces from file into list of provinces
	void LoadProvinces(string file_path) {
		ifstream input_file;
		input_file.open(file_path);

		if (!input_file) {
			cerr << "Unable to open file " << file_path;
			exit(1);   // call system to stop
		}

		//vector<vector<int>> adjacent_provinces;
		int line_number = 0;
		string line;
		while (getline(input_file, line)) {
			if (++line_number != 1) {
				//if the line is not the header, then create a new province
				int id = -1;
				string name = "Default Province";
				int terrain_id = -1;
				int r, g, b = 0;


				int index = line.find(",", 0);

				//get id column
				id = stoi(line.substr(0, index));
				line = line.substr(index + 1, line.length());
				index = line.find(",", 0);

				//get name column
				name = line.substr(0, index);
				line = line.substr(index + 1, line.length());
				index = line.find(",", 0);

				//get red color column
				r = stoi(line.substr(0, index));
				line = line.substr(index + 1, line.length());
				index = line.find(",", 0);

				//get green color column
				g = stoi(line.substr(0, index));
				line = line.substr(index + 1, line.length());
				index = line.find("\"", 0);

				//get blue color column
				b = stoi(line.substr(0, index));
				line = line.substr(index + 1, line.length());
				index = line.find("\"", 0);

				//get list of adjacent provinces
				string adjacent_provinces = line.substr(0, index);
				line = line.substr(index + 2, line.length());

				//get terrain id
				terrain_id = stoi(line);

				//create new terrain object to add to list at id index
				Province* new_province = new Province(name, glm::vec3(r,g,b), terrain_types.at(terrain_id));
				//make sure vector has enough indexes
				provinces.resize(max(id+1, (int)provinces.size()));
				//insert province to correct index
				provinces.at(id) = new_province;
			}
		}
		input_file.close();
	}

	//this function goes to the file location and turns the .bmp into provinces that it then adds to the map object
	void LoadMapMesh(string file_path) {
		//load csv of province data to initialize map
		cout << "Loading map from .bmp" << endl;

		const size_t color_channels = 3;
		//open bmp image to load province meshes
		//https://www.cplusplus.com/forum/beginner/267364/
		int width, height;
		std::vector<unsigned char> image;
		int n;

		//load image
		unsigned char* data = stbi_load(file_path.c_str(), &width, &height, &n, color_channels);
		if (data != nullptr)
		{
			image = vector<unsigned char>(data, data + width * height * color_channels);
		}
		stbi_image_free(data);

		//check if image loaded
		if (data == nullptr)
		{
			cerr << "Error loading image\n";
			exit(1);
		}

		glm::vec3 current_rgb = glm::vec3(0, 0, 0); 
		glm::vec3 previous_rgb = glm::vec3(0, 0, 0);

		//for each new rgb value look in list of provinces, compare province rgb value to new pixel rgb value if same send xy cords

		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {

				current_rgb = glm::vec3(image[color_channels * (j * width + i) + 0], image[color_channels * (j * width + i) + 1], image[color_channels * (j * width + i) + 2]);
				
				//std::cout << "Cords: " << i << ", " << j << ": " << current_rgb.r << ", " << current_rgb.g << ", " << current_rgb.b << std::endl;
				
				if (current_rgb != previous_rgb) {
					//we have seen a new pixel color
					//this pixel is an edge pixel and needs to be given to the province of the corresponding rgb

					//search provinces for matching new rgb value
					for (auto& p : provinces) {
						//if the rgb matches
						if (p->GetRGB() == current_rgb) {
							p->SendEdgeVertex(glm::vec3(i, -(j), 0));
						}
					}

					//search provinces for matching old rgb value
					for (auto& p : provinces) {
						//if the rgb matches
						if (p->GetRGB() == previous_rgb) {
							//y values are neagted
							p->SendEdgeVertex(glm::vec3(i, -(j - 1), 0));
						}
					}

				} else {
					// if the pixel color has not changed tell the province of that color the pixel is in its interior
					//search provinces for matching new rgb value
					for (auto& p : provinces) {
						//if the rgb matches
						if (p->GetRGB() == current_rgb) {
							p->SendInteriorVertex(glm::vec3(i, -(j), 0));
						}
					}
				}

				previous_rgb = current_rgb;
			}
			//system("pause");
		}

		//reset rgb values
		current_rgb = glm::vec3(0, 0, 0);
		previous_rgb = glm::vec3(0, 0, 0);

		//do the same thing but in a perpindicular direction
		for (int j = 0; j < height; j++) {
			for (int i = 0; i < width; i++) {

				current_rgb = glm::vec3(image[color_channels * (j * width + i) + 0], image[color_channels * (j * width + i) + 1], image[color_channels * (j * width + i) + 2]);
				
				//std::cout << "Cords: " << i << ", " << j << ": " << current_rgb.r << ", " << current_rgb.g << ", " << current_rgb.b << std::endl;
				
				if (current_rgb != previous_rgb) {
					//we have seen a new pixel color
					//this pixel is an edge pixel and needs to be given to the province of the corresponding rgb

					//search provinces for matching new rgb value
					for (auto& p : provinces) {
						//if the rgb matches
						if (p->GetRGB() == current_rgb) {
							p->SendEdgeVertex(glm::vec3(i, -j, 0));
							break;
						}
					}

					//search provinces for matching old rgb value
					for (auto& p : provinces) {
						//if the rgb matches
						if (p->GetRGB() == previous_rgb) {
							p->SendEdgeVertex(glm::vec3(i - 1, -j, 0));
							break;
						}
					}

				} else {
					// if the pixel color has not changed tell the province of that color the pixel is in its interior
					//search provinces for matching new rgb value
					for ( auto& p : provinces) {
						//if the rgb matches
						if (p->GetRGB() == current_rgb) {
							p->SendInteriorVertex(glm::vec3(i, -(j), 0));
						}
					}
				}

				previous_rgb = current_rgb;
			}
			//system("pause");
		}

		//tell each province to order its vertices
		int offset = 0;
		for (auto& p : provinces) {
			offset += p->CreateConnectivity(offset);
		}
	}
public:
	Map() {
		name = "default";
	}
	//go to folder and open necessary files to intialize provinces
	Map(string map_path) {
		LoadProvinceTerrain(map_path + "terrain_types.csv");
		LoadProvinces(map_path + "province_details.csv");
		LoadMapMesh(map_path + "province_map.bmp");

		cout << provinces.size() << endl;
		for (int i = 0; i < provinces.size(); i++) {
			Province* p = provinces.at(i);
			if (p != nullptr) {
				cout << "index: " << i << endl;
				cout << "name: " << p->GetName() << endl;
				cout << "rgb: " << p->GetRGB().r << ", " << p->GetRGB().g << ", " << p->GetRGB().b << endl;
				cout << "Terrain Type: " << p->GetProvinceTerrain()->GetTerrainName() << endl;
				cout << "	Terrain Movespeed: " << p->GetProvinceTerrain()->GetMovementSpeed() << endl;
			}
		}
	}

	//Vertices & Connectivity & Color visibility to engine
	vector<glm::vec3> get_map_vertice_data() {
		//ask each province, in order, for its vertices
		//assemble single vector of vertices, return to engine
		vector<glm::vec3> vertices;

		//for each province loop
		for (Province* &p : provinces) {
			//append provinces vertices to end of map veretices
			vertices.insert(vertices.end(), p->GetVertices()->begin(), p->GetVertices()->end());
		}
		return vertices;
	}

	vector<unsigned int> get_map_indice_data() {
		//ask each province, in order, for its indices
		//assemble single vector, return to engine
		vector<unsigned int> indices;

		//for each province loop
		for (Province*& p : provinces) {
			//append provinces vertices to end of map veretices
			indices.insert(indices.end(), p->GetIndices()->begin(), p->GetIndices()->end());
		}
		return indices;
	}

	vector<glm::vec4> get_map_color_data() {
		//ask each province for its color to display, return to engine
		vector<glm::vec4> color;

		//for each province loop
		for (Province*& p : provinces) {
			cout << "Getting color vector for: " << p->GetName() << endl;
			//append provinces vertices to end of map veretices
			vector<glm::vec4> provinceRGB = p->GetRGBArray();
			color.insert(color.end(), provinceRGB.begin(), provinceRGB.end());
		}
		return color;
	}

};