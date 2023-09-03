#pragma once

#include "myVector2D.h"
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
	int get_VectorIndex_from_cell(Vector2D i_j) {
		return i_j.X + i_j.Y * gridsize;
	}

	T get_cell_value(Vector2D i_j) {
		return this->cell_values[this->get_VectorIndex_from_cell(i_j)];
	}

	//cell vector index�� cell��ǥ�� ����
	int get_cell_i_from_VectorIndex(int vectorIndex) {
		return vectorIndex % gridsize;
	}

	//cell vector index�� cell��ǥ�� ����
	int get_cell_j_from_VectorIndex(int vectorIndex) {
		return vectorIndex / gridsize;
	}

	Vector2D get_cell_i_j_from_VectorIndex(int vectorIndex) {
		Vector2D result = Vector2D();
		result.X = vectorIndex % gridsize;
		result.Y = vectorIndex / gridsize;
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

	//������ǥ�� �޾Ƽ� �ѹ��� (i,j)�� return�ϴ� �Լ�
	Vector2D get_cell_i_j_from_world(Vector2D& particle) {
		Vector2D result = Vector2D();

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

		return result;

	}

	//cell ��ǥ�� ���� ���ǵ� cell �ȿ� �ִ��� Ȯ���ϴ� �Լ�
	double CLAMP(double d, double min, double max) {
		double t = d < min ? min : d;
		return t > max ? max : t;
	}


};
