#pragma once
#include <stdlib.h>
#include <vector>

struct Tile {
	int province_id;
	//int water_province_id;
};

struct Province {
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

float CalculateDistance(int x1, int y1, int x2, int y2) {
	return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2) * 1.0);
}

int ClosestProvince(int x, int y, std::vector<Province>* provinces) {
	int closest_province = provinces->at(0).id;
	float distance = CalculateDistance(x, y, provinces->at(0).x, provinces->at(0).y);

	for (int i = 1; i < provinces->size(); i++) {
		float new_distance = CalculateDistance(x, y, provinces->at(i).x, provinces->at(i).y);
		if (new_distance < distance) {
			closest_province = i;
			distance = new_distance;
		}
	}

	return closest_province;
}

// TODO: Update world gen with water tiles and terrain
bool GenerateMap(int province_count, int width, bool water = false, float water_ratio = 0.2) {
	int height = width / 2;

	std::vector<std::vector<Tile>> map;
	std::vector<Province> provinces;
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