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
	vector<glm::vec3> vertices;
	vector<unsigned int> indices;
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
			if (++line_number != 1) { //if the line is not the header, then create a new province
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

		glm::vec3 rgb = glm::vec3(0, 0, 0);

		// Create vertices
		for (int j = 0; j < height; j++) {
			for (int i = 0; i < width; i++) {
				rgb = glm::vec3(image[color_channels * (j * width + i) + 0], image[color_channels * (j * width + i) + 1], image[color_channels * (j * width + i) + 2]);
				glm::vec3 vertex = glm::vec3(i, -(j), 0);

				Province* p = GetProvinceByRGB(rgb);
				p->AddVertex(vertex);
			}
		}

		// Combine all province vertex vectors
		CollateProvinceVertexData();

		int a, b, c, d;
		for (int j = 0; j < height; j++) {
			for (int i = 0; i < width; i++) {
				//     c - x - x
				//     | \ | \ |
				//     a - b - x
				//     | \ | \ |
				//     x - d - x
				// create triangles: a-b-c & a-b-d
				//cout << "A" << endl;
				a = FindVertex(glm::vec3(i, -(j), 0));

				//does vertex b exist?
				if (i == width - 1) {
					continue;
				}
				else {
					//cout << "B" << endl;
					b = FindVertex(glm::vec3(i + 1, -j, 0));
				}

				//does vertex c exist?
				if (j != height - 1) {
					//cout << "C" << endl;
					c = FindVertex(glm::vec3(i, -(j + 1), 0));

					//create triangle C
					indices.push_back(a);
					indices.push_back(b);
					indices.push_back(c);
				}

				//does vertex d exist?
				if (j != 0) {
					//cout << "D" << endl;
					d = FindVertex(glm::vec3(i + 1, -(j - 1), 0));

					//create triangle D
					indices.push_back(a);
					indices.push_back(b);
					indices.push_back(d);
				}
			}
		}
	}
	Province* GetProvinceByRGB(glm::vec3 rgb) {
		for (auto& p : provinces) {
			if (p->GetRGB() == rgb) {
				return p;
			}
		}

		return nullptr;
	}

	void CollateProvinceVertexData() {
		//ask each province, in order, for its vertices
		//assemble single vector of vertices, return to engine

		//for each province loop
		for (Province*& p : provinces) {
			//append provinces vertices to end of map veretices
			vertices.insert(vertices.end(), p->GetVertices()->begin(), p->GetVertices()->end());
		}
	}

	int FindVertex(glm::vec3 vertex) {
		int i = std::distance(vertices.begin(), std::find(vertices.begin(), vertices.end(), vertex));

		if (std::find(vertices.begin(), vertices.end(), vertex) == vertices.end()) {
			cout << "VERTEX NOT FOUND" << endl;
			cout << "x:" << vertex.x << ", y:" << vertex.y << ", z:" << vertex.z << endl;

			cout << vertices.size() << endl;

			system("pause");
		}

		return i;
	}

public:
	Map() {
		name = "default";
	}
	//go to folder and open necessary files to intialize provinces
	Map(string map_path) {
		LoadProvinceTerrain(map_path + "terrain_types.csv");
		LoadProvinces(map_path + "province_details.csv");
		LoadMapMesh(map_path + "equal_projection.bmp");

		cout << "map provinces #" << provinces.size() << endl;
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
		return vertices;
	}

	vector<unsigned int> get_map_indice_data() {
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
	vector<glm::vec3>* GetVertices() {
		return &vertices;
	}
};