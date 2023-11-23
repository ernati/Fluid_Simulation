#pragma once

#include "myVector2D.h"
#include <vector>

using namespace std;

//MAC_Grid는 Marker-And-Cell Grid의 줄인말로,
//유체 시뮬레이션에 쓰이는 cell 혹은 voxel로 이루어진 Grid를 지칭하는 용어이다.
//MAC cell 혹은 voxel은, 인접한 cell과 voxel과의 관계변수를 담는 cell이다.
template <class T>
class MAC_Grid {

public:

	//gridsize가 n이면 nxn grid다
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

	//이 함수를 구현한 이유는, cell_values를 private으로 하기 위함이었으나, 
	//구현하다보니, 편의를 위해 사용하지 않았다.
	//실무에 들어가면 꼭 주의해야지.
	T get_cell_value(Vector2D i_j) {
		return this->cell_values[this->get_VectorIndex_from_cell(i_j)];
	}

	//cell vector index와 대응하는 cell의 i좌표를 리턴
	int get_cell_i_from_VectorIndex(int vectorIndex) {
		return vectorIndex % gridsize;
	}

	//cell vector index와 대응하는 cell의 j좌표를 리턴
	int get_cell_j_from_VectorIndex(int vectorIndex) {
		return vectorIndex / gridsize;
	}

	//vector index와 대응하는 (i,j) cell 좌표를 리턴
	Vector2D get_cell_i_j_from_VectorIndex(int vectorIndex) {
		Vector2D result = Vector2D();
		result.X = vectorIndex % gridsize;
		result.Y = vectorIndex / gridsize;
		return result;
	}

	// particle의 world_x 좌표와 대응하는 cell의 i좌표를 리턴
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

	// particle의 world_y 좌표와 대응하는 cell의 j좌표를 리턴
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

	//월드좌표를 받아서 한번에 (i,j)를 return하는 함수
	Vector2D get_cell_i_j_from_world(Vector2D& particle) {
		Vector2D result = Vector2D();

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

		return result;

	}

	//cell 좌표가 현재 정의된 cell 안에 있는지 확인하는 함수
	double CLAMP(double d, double min, double max) {
		double t = d < min ? min : d;
		return t > max ? max : t;
	}


};
