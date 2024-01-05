#pragma once
#include <vector>
#include <cstdlib>
#include <iostream>

unsigned int Power(unsigned int a, unsigned int b) {
	unsigned int result = 1;
	for (unsigned int i = 0; i < b; ++i) {
		result *= a;
	}
	return result;
}

std::vector<std::vector<double>> DiamondSquare(int n, double tl, double tr, double bl, double br, double random_scale) {
	unsigned int vertices_n = Power(2, n) + 1;

	std::vector<std::vector<double>> height_map(vertices_n, std::vector<double>(vertices_n));

	// (0,0) --- (1,0)
	//   |         |
	// (0,1) --- (1,1)

	height_map.at(0).at(0) = tl;
	height_map.at(vertices_n - 1).at(0) = tr;
	height_map.at(0).at(vertices_n - 1) = bl;
	height_map.at(vertices_n - 1).at(vertices_n - 1) = br;


	for (unsigned int i = 0, step_size = vertices_n - 1; i < n; ++i, step_size /= 2, random_scale /= 2) {
		// Diamond
		for (unsigned int j = 0; j < (vertices_n - 1) / step_size; ++j) {
			for (unsigned int k = 0; k < (vertices_n - 1) / step_size; ++k) {
				unsigned int tl_x = k * step_size;			unsigned int tl_y = j * step_size;
				unsigned int tr_x = (k + 1) * step_size;	unsigned int tr_y = j * step_size;
				unsigned int bl_x = k * step_size;			unsigned int bl_y = (j + 1) * step_size;
				unsigned int br_x = (k + 1) * step_size;	unsigned int br_y = (j + 1) * step_size;

				unsigned int middle_x = (tl_x + tr_x + bl_x + br_x) / 4;
				unsigned int middle_y = (tl_y + tr_y + bl_y + br_y) / 4;

				height_map.at(middle_x).at(middle_y) = (
					height_map.at(tl_x).at(tl_y) + 
					height_map.at(tr_x).at(tr_y) +
					height_map.at(bl_x).at(bl_y) +
					height_map.at(br_x).at(br_y)
				) / 4 + (random_scale * (rand() % 20000 / 10000.0 - 1));
			}
		}

		// Square
		for (unsigned int j = 0; j < (vertices_n - 1) / step_size; ++j) {
			for (unsigned int k = 0; k < (vertices_n - 1) / step_size; ++k) {
				unsigned int tl_x = k * step_size;			unsigned int tl_y = j * step_size;
				unsigned int tr_x = (k + 1) * step_size;	unsigned int tr_y = j * step_size;
				unsigned int bl_x = k * step_size;			unsigned int bl_y = (j + 1) * step_size;
				unsigned int br_x = (k + 1) * step_size;	unsigned int br_y = (j + 1) * step_size;

				unsigned int middle_x = (tl_x + tr_x + bl_x + br_x) / 4;
				unsigned int middle_y = (tl_y + tr_y + bl_y + br_y) / 4;

				unsigned int t_x = middle_x;					unsigned int t_y = middle_y - (step_size / 2);
				unsigned int r_x = middle_x + (step_size / 2);	unsigned int r_y = middle_y;
				unsigned int b_x = middle_x;					unsigned int b_y = middle_y + (step_size / 2);
				unsigned int l_x = middle_x - (step_size / 2);	unsigned int l_y = middle_y;

				unsigned int rr_x = r_x + (step_size / 2);	unsigned int rr_y = r_y;
				unsigned int bb_x = b_x;					unsigned int bb_y = b_y + (step_size / 2);

				if (j == 0) {
					height_map.at(t_x).at(t_y) = (
						height_map.at(tl_x).at(tl_y) +
						height_map.at(tr_x).at(tr_y) +
						height_map.at(middle_x).at(middle_y)
					) / 3 + (random_scale * (rand() % 20000 / 10000.0 - 1));
				}
				if (rr_x >= 0 && rr_x <= vertices_n - 1 && rr_y >= 0 && rr_y <= vertices_n - 1) {
					height_map.at(r_x).at(r_y) = (
						height_map.at(br_x).at(br_y) +
						height_map.at(tr_x).at(tr_y) +
						height_map.at(rr_x).at(rr_y) +
						height_map.at(middle_x).at(middle_y)
					) / 4 + (random_scale * (rand() % 20000 / 10000.0 - 1));
				}
				else {
					height_map.at(r_x).at(r_y) = (
						height_map.at(br_x).at(br_y) +
						height_map.at(tr_x).at(tr_y) +
						height_map.at(middle_x).at(middle_y)
					) / 3 + (random_scale * (rand() % 20000 / 10000.0 - 1));
				}
				if (bb_x >= 0 && bb_x <= vertices_n - 1 && bb_y >= 0 && bb_y <= vertices_n - 1) {
					height_map.at(b_x).at(b_y) = (
						height_map.at(br_x).at(br_y) +
						height_map.at(bl_x).at(bl_y) +
						height_map.at(bb_x).at(bb_y) +
						height_map.at(middle_x).at(middle_y)
					) / 4 + (random_scale * (rand() % 20000 / 10000.0 - 1));
				}
				else {
					height_map.at(b_x).at(b_y) = (
						height_map.at(br_x).at(br_y) +
						height_map.at(bl_x).at(bl_y) +
						height_map.at(middle_x).at(middle_y)
					) / 3 + (random_scale * (rand() % 20000 / 10000.0 - 1));
				}
				if (k == 0) {
					height_map.at(l_x).at(l_y) = (
						height_map.at(tl_x).at(tl_y) +
						height_map.at(bl_x).at(bl_y) +
						height_map.at(middle_x).at(middle_y)
					) / 3 + (random_scale * (rand() % 20000 / 10000.0 - 1));
				}
			}
		}
	}

	return height_map;
}