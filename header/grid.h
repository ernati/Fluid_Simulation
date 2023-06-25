#pragma once

#include "myVector2D.h"
#include "particle.h"
#include <vector>

template <class T>
class MAC_Grid {

public:

	int gridsize;
	//값들을 담을 vector
	vector<T> cell_values;

	MAC_Grid() {

	}

	MAC_Grid(int grid_N) {
		gridsize = grid_N;
	}

	//cell좌표를 vector index로 변경
	int get_VectorIndex_from_cell(Vector2D i_j) {
		return i_j.X + i_j.Y * gridsize;
	}

	//cell vector index를 cell좌표로 변경
	int get_cell_i_from_VectorIndex(int vectorIndex) {
		return vectorIndex % gridsize;
	}

	//cell vector index를 cell좌표로 변경
	int get_cell_j_from_VectorIndex(int vectorIndex) {
		return vectorIndex / gridsize;
	}

	// particle의 world_x 좌표를 cell 좌표로 변경
	int get_cell_i_from_world(float particle_x) {
		for (int i = 0; i < gridsize; i++) {
			//ex 0.6 보다 클 때,
			if (1.0 / (float)gridsize * i < particle_x) {
				//ex 0.7보다 작으면
				if (1.0 / (float)gridsize * (i + 1) > particle_x) {
					return i;
				}

			}
		}
	}

	// particle의 world_y 좌표를 cell 좌표로 변경
	int get_cell_j_from_world(float particle_y) {
		for (int j = 0; j < gridsize; j++) {
			//ex 0.6 보다 클 때,
			if (1.0 / (float)gridsize * j < particle_y) {
				//ex 0.7보다 작으면
				if (1.0 / (float)gridsize * (j + 1) > particle_y) {
					return j;
				}
			}
		}
	}

	//월드좌표를 받아서 한번에 (i,j)를 return하는 함수
	Vector2D get_cell_i_j_from_world(Vector2D particle) {
		Vector2D result = Vector2D();

		for (int i = 0; i < gridsize; i++) {
			//ex 0.6 보다 클 때,
			if (1.0 / (float)gridsize * i < particle.X) {
				//ex 0.7보다 작으면
				if (1.0 / (float)gridsize * (i + 1) > particle.X) {
					result.X = (float)i;
				}
			}
		}

		for (int j = 0; j < gridsize; j++) {
			//ex 0.6 보다 클 때,
			if (1.0 / (float)gridsize * j < particle.Y) {
				//ex 0.7보다 작으면
				if (1.0 / (float)gridsize * (j + 1) > particle.Y) {
					result.Y = (float)j;
				}
			}
		}

		return result;

	}

	//cell 좌표가 현재 정의된 cell 안에 있는지 확인하는 함수
	float CLAMP(float d, float min, float max) {
		float t = d < min ? min : d;
		return t > max ? max : t;
	}


};
