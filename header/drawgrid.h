#pragma once
#include <vector>

#include "quadratic_particle.h"

using namespace std;


void make_Points_of_grids( vector<Vector2D> &grid, int n ) {

	float j = 1;
	//가로줄
	for (int i = 0; i < n-1; i++) {
		
		grid[2 * i] = Vector2D(0, 1 /(float)n * (float)j);
		grid[2 * i + 1] = Vector2D(1, 1 / (float)n * (float)j);
		j = j + 1;
		
	}

	j = 1;

	//세로줄
	for (int i = n - 1; i < 2 * (n - 1); i++) {
		grid[2 * i] = Vector2D(1 / (float)n * (float)j, 0);
		grid[2 * i + 1] = Vector2D(1 / (float)n * (float)j,1);
		j = j + 1;
	}

}