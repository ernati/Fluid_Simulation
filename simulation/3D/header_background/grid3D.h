#pragma once

#include "myVector3D.h"
#include <vector>

using namespace std;

template <class T>
class MAC_Grid {

public:

	int gridsize;
	//������ ���� vector
	vector<T> cell_values;

	MAC_Grid() {

	}

	MAC_Grid(int grid_N) {
		gridsize = grid_N;
	}

	//cell��ǥ�� vector index�� ����
	int get_VectorIndex_from_cell(Vector3D& i_j_k) {
		return i_j_k.X + i_j_k.Y * gridsize + i_j_k.Z * gridsize * gridsize;
	}

	//cell vector index�� cell��ǥ�� ����
	int get_cell_i_from_VectorIndex(int vectorIndex) {
		return ( vectorIndex % (gridsize * gridsize) ) % gridsize;
	}

	//cell vector index�� cell��ǥ�� ����
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

	// particle�� world_x ��ǥ�� cell ��ǥ�� ����
	int get_cell_i_from_world(double particle_x) {
		for (int i = 0; i < gridsize; i++) {
			//ex 0.6 ���� Ŭ ��,
			if (1.0 / (double)gridsize * i < particle_x) {
				//ex 0.7���� ������
				if (1.0 / (double)gridsize * (i + 1) > particle_x) {
					return i;
				}

			}
		}
	}

	// particle�� world_y ��ǥ�� cell ��ǥ�� ����
	int get_cell_j_from_world(double particle_y) {
		for (int j = 0; j < gridsize; j++) {
			//ex 0.6 ���� Ŭ ��,
			if (1.0 / (double)gridsize * j < particle_y) {
				//ex 0.7���� ������
				if (1.0 / (double)gridsize * (j + 1) > particle_y) {
					return j;
				}
			}
		}
	}

	// particle�� world_z ��ǥ�� cell ��ǥ�� ����
	int get_cell_k_from_world(double particle_z) {
		for (int k = 0; k < gridsize; k++) {
			//ex 0.6 ���� Ŭ ��,
			if (1.0 / (double)gridsize * k < particle_z) {
				//ex 0.7���� ������
				if (1.0 / (double)gridsize * (k + 1) > particle_z) {
					return k;
				}
			}
		}
	}

	//������ǥ�� �޾Ƽ� �ѹ��� (i,j,k)�� return�ϴ� �Լ�
	Vector3D get_cell_i_j_k_from_world(Vector3D& particle) {
		Vector3D result = Vector3D();

		for (int i = 0; i < gridsize; i++) {
			//ex 0.6 ���� Ŭ ��,
			if (1.0 / (double)gridsize * i < particle.X) {
				//ex 0.7���� ������
				if (1.0 / (double)gridsize * (i + 1) > particle.X) {
					result.X = (double)i;
				}
			}
		}

		for (int j = 0; j < gridsize; j++) {
			//ex 0.6 ���� Ŭ ��,
			if (1.0 / (double)gridsize * j < particle.Y) {
				//ex 0.7���� ������
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

	//cell ��ǥ�� ���� ���ǵ� cell �ȿ� �ִ��� Ȯ���ϴ� �Լ�
	double CLAMP(double d, double min, double max) {
		double t = d < min ? min : d;
		return t > max ? max : t;
	}


};
