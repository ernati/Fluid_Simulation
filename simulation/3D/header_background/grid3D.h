#pragma once

#include "myVector3D.h"
#include <vector>

using namespace std;

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
	int get_VectorIndex_from_cell(Vector3D& i_j_k) {
		return i_j_k.X + i_j_k.Y * gridsize + i_j_k.Z * gridsize * gridsize;
	}

	//cell vector index를 cell좌표로 변경
	int get_cell_i_from_VectorIndex(int vectorIndex) {
		return ( vectorIndex % (gridsize * gridsize) ) % gridsize;
	}

	//cell vector index를 cell좌표로 변경
	int get_cell_j_from_VectorIndex(int vectorIndex) {
		return (vectorIndex % (gridsize * gridsize)) / gridsize;
	}

	int get_cell_k_from_VectorIndex(int vectorIndex) {
		return vectorIndex / (gridsize * gridsize);
	}

	Vector3D get_cell_i_j_k_from_VectorIndex(int vectorIndex) {
		Vector3D result = Vector3D();

		result.X = (vectorIndex % (gridsize * gridsize)) % gridsize;
		result.Y = (vectorIndex % (gridsize * gridsize)) / gridsize;
		result.Z = vectorIndex / (gridsize * gridsize);

		return result;
	}

	// particle의 world_x 좌표를 cell 좌표로 변경
	int get_cell_i_from_world(double particle_x) {
		for (int i = 0; i < gridsize; i++) {
			//ex 0.6 보다 클 때,
			if (1.0 / (double)gridsize * i < particle_x) {
				//ex 0.7보다 작으면
				if (1.0 / (double)gridsize * (i + 1) > particle_x) {
					return i;
				}

			}
		}
	}

	// particle의 world_y 좌표를 cell 좌표로 변경
	int get_cell_j_from_world(double particle_y) {
		for (int j = 0; j < gridsize; j++) {
			//ex 0.6 보다 클 때,
			if (1.0 / (double)gridsize * j < particle_y) {
				//ex 0.7보다 작으면
				if (1.0 / (double)gridsize * (j + 1) > particle_y) {
					return j;
				}
			}
		}
	}

	// particle의 world_z 좌표를 cell 좌표로 변경
	int get_cell_k_from_world(double particle_z) {
		for (int k = 0; k < gridsize; k++) {
			//ex 0.6 보다 클 때,
			if (1.0 / (double)gridsize * k < particle_z) {
				//ex 0.7보다 작으면
				if (1.0 / (double)gridsize * (k + 1) > particle_z) {
					return k;
				}
			}
		}
	}

	//월드좌표를 받아서 한번에 (i,j,k)를 return하는 함수
	Vector3D get_cell_i_j_k_from_world(Vector3D& particle) {
		Vector3D result = Vector3D();

		for (int i = 0; i < gridsize; i++) {
			//ex 0.6 보다 클 때,
			if (1.0 / (double)gridsize * i < particle.X) {
				//ex 0.7보다 작으면
				if (1.0 / (double)gridsize * (i + 1) > particle.X) {
					result.X = (double)i;
				}
			}
		}

		for (int j = 0; j < gridsize; j++) {
			//ex 0.6 보다 클 때,
			if (1.0 / (double)gridsize * j < particle.Y) {
				//ex 0.7보다 작으면
				if (1.0 / (double)gridsize * (j + 1) > particle.Y) {
					result.Y = (double)j;
				}
			}
		}

		for (int k = 0; k < gridsize; k++) {
			if (1.0 / (double)gridsize * k < particle.Z) {
				if (1.0 / (double)gridsize * (k + 1) > particle.Z) {
					result.Z = (double)k;
				}
			}
		}

		return result;

	}

	//cell 좌표가 현재 정의된 cell 안에 있는지 확인하는 함수
	double CLAMP(double d, double min, double max) {
		double t = d < min ? min : d;
		return t > max ? max : t;
	}


};
