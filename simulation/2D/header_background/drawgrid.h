#pragma once
#include <vector>

#include "particle.h"

using namespace std;

//뷰포트에 n에 따른 grid 선분들을 구성하는 끝 점들을 생성하는 함수
//n=10이면, 그리드 칸이 10x10이므로, 가로 선분 11개, 세로선분 11개가 생성된다.
//즉 끝점은 44개가 된다.
void make_Points_of_grids( vector<Vector2D> &grid, int n ) {

	double j = 1;
	//가로줄
	for (int i = 0; i < n-1; i++) {
		
		grid[2 * i] = Vector2D(0, 1 /(double)n * (double)j);
		grid[2 * i + 1] = Vector2D(1, 1 / (double)n * (double)j);
		j = j + 1;
		
	}

	j = 1;

	//세로줄
	for (int i = n - 1; i < 2 * (n - 1); i++) {
		grid[2 * i] = Vector2D(1 / (double)n * (double)j, 0);
		grid[2 * i + 1] = Vector2D(1 / (double)n * (double)j,1);
		j = j + 1;
	}

}
