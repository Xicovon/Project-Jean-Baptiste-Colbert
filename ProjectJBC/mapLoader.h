#pragma once
#include "map.h"
#include "glm.hpp"
//#include "stb_image.h"
#include <iostream>
#include <vector>
#include <string>
//this file contains functions to transform .bmp images into a series of glm meshes


extern "C" {
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
}

const size_t color_channels = 3;

// Loads as RGBA... even if file is only RGB
// Feel free to adjust this if you so please, by changing the 4 to a 0.
bool load_image(std::vector<unsigned char>& image, const std::string& filename, int& x, int& y)
{
	int n;
	unsigned char* data = stbi_load(filename.c_str(), &x, &y, &n, color_channels);
	if (data != nullptr)
	{
		image = std::vector<unsigned char>(data, data + x * y * color_channels);
	}
	stbi_image_free(data);
	return (data != nullptr);
}

//this function goes to the file location and turns the .bmp into provinces that it then adds to the map object
Map* LoadMap(string map_path) {
	//load csv of province data to initialize map
	Map* map = new Map(map_path);


	//open bmp image to load province meshes
	//https://www.cplusplus.com/forum/beginner/267364/
	int width, height;
	std::vector<unsigned char> image;

	bool success = load_image(image, map_path + "province_map.bmp", width, height);
	if (!success)
	{
		std::cout << "Error loading image\n";
		return nullptr;
	}

	glm::vec3 current_rgb = glm::vec3(0, 0, 0);
	glm::vec3 previous_rgb = glm::vec3(0, 0, 0);

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			
			current_rgb = glm::vec3(image[color_channels * (i * width + j) + 0], image[color_channels * (i * width + j) + 1], image[color_channels * (i * width + j) + 2]);
			//std::cout << current_rgb.r << ", " << current_rgb.g << ", " << current_rgb.b << std::endl;

			previous_rgb = current_rgb;
		}
	}

	
	//return map to engine
	return map;
}