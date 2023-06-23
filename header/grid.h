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
	int get_VectorIndex_from_cell(int i, int j) {
		return i + j * gridsize;
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
			if (1.0 / (float)gridsize * i > particle_x) {
				//ex 0.7보다 작으면
				if (1.0 / (float)gridsize * (i + 1) < particle_x) {
					return i;
				}

			}
		}
	}

	// particle의 world_y 좌표를 cell 좌표로 변경
	int get_cell_j_from_world(float particle_y) {
		for (int j = 0; j < gridsize; j++) {
			//ex 0.6 보다 클 때,
			if (1.0 / (float)gridsize * j > particle_y) {
				//ex 0.7보다 작으면
				if (1.0 / (float)gridsize * (j + 1) < particle_y) {
					return j;
				}
			}
		}
	}


};
