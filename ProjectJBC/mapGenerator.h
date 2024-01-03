#pragma once
#include <stdlib.h>
#include <vector>
#include <diamondSquare.h>
#include <string>
#include <iostream>

struct Tile {
	int province_id;
	//int water_province_id;
};

struct GeneratorProvince {
	int id;
	int x;
	int y;
	//int water_province_id;
};

int Min(int a, int b) {
	return a < b ? a : b;
}

int TileCount(int tiles_remaining, int provinces_remaining) {
	int max_variation = Min(10, tiles_remaining / 10);
	int variation = rand() % max_variation;
	return (tiles_remaining / provinces_remaining) + variation;
}

double CalculateDistance(int x1, int y1, int x2, int y2) {
	return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2) * 1.0);
}

int ClosestProvince(int x, int y, std::vector<GeneratorProvince>* provinces) {
	int closest_province = provinces->at(0).id;
	float distance = CalculateDistance(x, y, provinces->at(0).x, provinces->at(0).y);

	for (unsigned int i = 1; i < provinces->size(); i++) {
		float new_distance = CalculateDistance(x, y, provinces->at(i).x, provinces->at(i).y);
		if (new_distance < distance) {
			closest_province = i;
			distance = new_distance;
		}
	}

	return closest_province;
}

// TODO: Update world gen with water tiles and terrain
void GenerateMap(int province_count, int width, bool water = false, float water_ratio = 0.2) {
	int height = width / 2;

	std::vector<std::vector<Tile>> map;
	std::vector<GeneratorProvince> provinces;
	//std::vector<Province> water_provinces;

	//int water_provinces_count = water ? water_ratio * province_count : 0;
	//int initial_province_count = province_count + (water_provinces_count * 10);

	for (int i = 0; i < province_count; i++) {
		provinces.at(i).id = i;
		provinces.at(i).x = rand() % (width - 1);
		provinces.at(i).y = rand() % (height - 1);
	}

	for (int j = 0; j < width; j++) {
		for (int k = 0; k < height; k++) {
			map.at(j).at(k).province_id = ClosestProvince(j, k, &provinces);
		}
	}

	//for (int i = 0; i < water_provinces_count; i++) {
	//	water_provinces.at(i).id = i;
	//	water_provinces.at(i).x = rand() % (width - 1);
	//	water_provinces.at(i).y = rand() % (height - 1);
	//}

}

void PopulateNoiseMap(std::vector<std::vector<int>>* empty_map, int height, int width) {
	for (int i = 0; i > height; i++) {
		for (int j = 0; j < width; j++) {
			empty_map->at(i).at(j) = (int)rand() % 100; // random number from 0 to 99
		}
	}
}

std::vector<std::vector<double>> GenerateMap(int width) {
	int height = width / 2;
	std::vector<std::vector<int>>* noise_map = new std::vector<std::vector<int>>;
	PopulateNoiseMap(noise_map, height, width);

	// do heightmap to determine where water (oceans & lakes * unpassable mountains)
	std::vector<std::vector<double>> height_map = DiamondSquare(width, rand() % 1000, rand() % 1000, rand() % 1000, rand() % 1000, 1);

	// do voronoi noise to create land terrain

	return height_map;
}

