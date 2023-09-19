#pragma once
#include <string>
#include "provinceTerrain.h"
#include <vector>
using namespace std;

//Geographical
//- Planet
//  - Region
//    - Area
//      - Province

class Province {
private:
	string name;
	ProvinceTerrain* terrain_type;
	vector<Province*> adjacent_provinces; //list of pointers to connected provinces
	glm::vec3 rgb;
	

	vector<glm::vec3> allVertices;
	vector<glm::vec3> edgeVertices;
	//vector<glm::vec3> normals;
	//vector<glm::vec2> UVs;
	vector<unsigned int> indices;

	//mesh of vertices/shape
	//owner: reference to occupying faction
	//economic factors
	//list of references to population units
	//list of references to resource objects and their value as %
	//list of improvements: buildings/roads

	//searches the vertex vector for the coordinates
	bool findVertex(glm::vec3 vertexToFind, vector<glm::vec3>* usedVertices) {
		//cout << "Finding vertex: " << vertexToFind.x << ", " << vertexToFind.y << endl;
		for (int i = 0; i < edgeVertices.size(); i++) {
			if (edgeVertices.at(i) == vertexToFind) {
				//the vertex has been found now check if it has already been used
				for (auto& v : *usedVertices) {
					if (v == edgeVertices.at(i)) {
						//if they match the vertex has already been found and it is skipped
						return false;
					}
				}
				//remove the vertex from the old vector and add it to the new vector
				edgeVertices.erase(edgeVertices.begin() + i);
				usedVertices->push_back(vertexToFind);
				//if the vertex has not already been used then return true to use the vertex
				return true;
			}
		}
		//the vertex is a miss, go to next
		return false;
	}


	//TODO:
	bool findIndice(glm::vec3 indiceToFind, vector<glm::vec3>* indices) {

	}

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
	//provides access to vertices and indices for the map to aggregate and send back to engine
	vector<glm::vec3>* GetVertices() {
		return &allVertices;
	}
	vector<unsigned int>* GetIndices() {
		return &indices;
	}
	//receive new vertices when map loader is reading map img
	void SendInteriorVertex(glm::vec3 newVertex) {
		//check if pixel is already in list
		for (auto& vertex : allVertices) {
			if (vertex == newVertex) {
				return;
			}
		}
		//if the vertex is unique add it.
		allVertices.push_back(newVertex);
	}
	void SendEdgeVertex(glm::vec3 newVertex) {
		//check if pixel is already in list
		for (auto& vertex : edgeVertices) {
			if (vertex == newVertex) {
				return;
			}
		}
		//if the vertex is unique add it.
		edgeVertices.push_back(newVertex);
		SendInteriorVertex(newVertex);
	}

	vector<glm::vec4> GetRGBArray() {
		vector<glm::vec4> colorArray;
		for (int i = 0; i < allVertices.size(); i++) {
			colorArray.push_back(glm::vec4(rgb.r, rgb.g, rgb.b, 1.0));
		}
		return colorArray;
	}

	//function to connect vertices into triangles and store in indices
	//takes the number of vertices from provinces before it
	//returns number of vertices in this province
	int CreateConnectivity(int vertexOffset) {
		/*cout << "creating connectivity for: " << name << endl;
		//loop through vertices and put them in order

		vector<glm::vec3> tempVertices;

		//first vertex is first in vector
		glm::vec3 v = edgeVertices.at(0);
		glm::vec3 startVertex = v;

		//remove first element from vertices and add to new vertices
		tempVertices.push_back(v);
		edgeVertices.erase(edgeVertices.begin());

		while (edgeVertices.size() > 0) {
		//for (auto &v : vertices) {
			//for each vertex
			//cout << "connecting pixel: " << v.x << ", " << v.y << endl;

			//find next vertex in chain starting counter clockwise
			if (findVertex(glm::vec3(v.x, v.y - 1, v.z), &tempVertices)) {
				v = glm::vec3(v.x, v.y - 1, v.z);

			}else if (findVertex(glm::vec3(v.x - 1, v.y - 1, v.z), &tempVertices)) {
				v = glm::vec3(v.x - 1, v.y - 1, v.z);

			}else if (findVertex(glm::vec3(v.x - 1, v.y, v.z), &tempVertices)) {
				v = glm::vec3(v.x - 1, v.y, v.z);

			}else if (findVertex(glm::vec3(v.x - 1, v.y + 1, v.z), &tempVertices)) {
				v = glm::vec3(v.x - 1, v.y + 1, v.z);

			}else if (findVertex(glm::vec3(v.x, v.y + 1, v.z), &tempVertices)) {
				v = glm::vec3(v.x, v.y + 1, v.z);

			}else if (findVertex(glm::vec3(v.x + 1, v.y + 1, v.z), &tempVertices)) {
				v = glm::vec3(v.x + 1, v.y + 1, v.z);

			}else if (findVertex(glm::vec3(v.x+1, v.y, v.z), &tempVertices)) {
				v = glm::vec3(v.x + 1, v.y, v.z);

			}else if (findVertex(glm::vec3(v.x + 1, v.y - 1, v.z), &tempVertices)) {
				v = glm::vec3(v.x + 1, v.y - 1, v.z);
			}
			else {
				//if no next vertex can be found reset start vertex
				//this would be needed for multiple independant hulls
				v = edgeVertices.at(0);
				//cout << "no next pixel found" << endl;
			}
			//cout << "new v: " << v.x << ", " << v.y << endl;
			//system("pause");
		}
		//replace old vertices with new ordered vertices
		edgeVertices = tempVertices;*/

		//new tesselation algorithm
		//traverse all vertices creating vec3 triangles
		//
		// x   x   x
		//		 / |
		// x   x - x
		//   / | /
		// x - x   x
		// if (0,-1) does not exist try to include (1,-1) with (0,0) and (1,0)


		return allVertices.size();
	}
};