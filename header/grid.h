#pragma once

#include "myVector2D.h"
#include "particle.h"
#include <vector>

template <class T>
class MAC_Grid {

public:

	int gridsize;
	//������ ���� vector
	vector<T> cell_values;

	MAC_Grid() {

	}

	MAC_Grid( int grid_N ) {
		gridsize = grid_N;
	}

	//cell��ǥ�� vector index�� ����
	int get_VectorIndex_from_cell(int i, int j) {
		return i + j * gridsize;
	}

	//cell vector index�� cell��ǥ�� ����
	int get_cell_i_from_VectorIndex(int vectorIndex) {
		return i % gridsize;
	}

	//cell vector index�� cell��ǥ�� ����
	int get_cell_j_from_VectorIndex(int vectorIndex) {
		return vectorIndex / gridsize;
	}

	// particle�� world_x ��ǥ�� cell ��ǥ�� ����
	int get_cell_i_from_world(float particle_x) {
		for (int i = 0; i < gridsize; i++) {
			//ex 0.6 ���� Ŭ��, 
			if ((1.0 / (float)gridsize * i > particle_x) {
				//ex 0.7���� ������
				if (1.0 / (float)gridsize * (i + 1) < particle_x) {
					return i;
				}

			}
		}
	}

	// particle�� world_y ��ǥ�� cell ��ǥ�� ����
	int get_cell_i_from_world(float particle_y) {
		for (int j = 0; j < gridsize; j++) {
			//ex 0.6 ���� Ŭ��, 
			if ((1.0 / (float)gridsize * j > particle_x) {
				//ex 0.7���� ������
				if (1.0 / (float)gridsize * (j + 1) < particle_x) {
					return j;
				}
			}
		}
	}

	
};
