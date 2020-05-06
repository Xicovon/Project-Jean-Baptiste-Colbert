#pragma once
#include "map.h"
#include "glm.hpp"
//#include "stb_image.h"
#include <iostream>
#include <vector>

//this file contains functions to transform .bmp images into a series of glm meshes



extern "C" {
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
}

// Loads as RGBA... even if file is only RGB
// Feel free to adjust this if you so please, by changing the 4 to a 0.
bool load_image(std::vector<unsigned char>& image, const std::string& filename, int& x, int& y)
{
	int n;
	unsigned char* data = stbi_load(filename.c_str(), &x, &y, &n, 3);
	if (data != nullptr)
	{
		image = std::vector<unsigned char>(data, data + x * y * 3);
	}
	stbi_image_free(data);
	return (data != nullptr);
}

//this function goes to the file location and turns the .bmp into provinces that it then adds to the map object
bool LoadMap(Map* map, const char* map_path) {

	//https://www.cplusplus.com/forum/beginner/267364/
	int width, height;
	std::vector<unsigned char> image;
	bool success = load_image(image, map_path, width, height);
	if (!success)
	{
		std::cout << "Error loading image\n";
		return 1;
	}

	std::cout << "Image width = " << width << '\n';
	std::cout << "Image height = " << height << '\n';

	const size_t RGB = 3;

	int x = 50;
	int y = 50;
	size_t index = RGB * (y * width + x);
	std::cout << "RGB pixel @ (x=50, y=50): "
		<< static_cast<int>(image[index + 0]) << " "
		<< static_cast<int>(image[index + 1]) << " "
		<< static_cast<int>(image[index + 2]) << std::endl;

	//for (int i = 0; i < y; i++) {
	//	for (int j = 0; j < x; j++) {
	//		if (j == 50) {
	//			std::cout << "Row " << i << " Column " << j << " ("
	//				<< static_cast<int>(data[(i * n * x) + j]) << ", "
	//				<< static_cast<int>(data[(i * n * x) + j + 1]) << ", "
	//				<< static_cast<int>(data[(i * n * x) + j + 2]) << ") " << std::endl;
	//				/*<< ((i * n * x) + (j * n)) << ", "
	//				<< ((i * n * x) + (j * n) + 1) << ", "
	//				<< ((i * n * x) + (j * n) + 2) << ") " << std::endl;*/
	//		}
	//	}
	//}

	return true;
}