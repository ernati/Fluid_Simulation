#pragma once
#include <vector>
#include "particle3D.h"


using namespace std;

//총 갯수는 4* (n+1)^2 -> 
vector<Vector3D>* make_Points_of_grids(int n) {

	vector< Vector3D >tmp_z_0;
	vector< Vector3D >tmp_z_n;

	vector< Vector3D >tmp_x_0;
	vector< Vector3D >tmp_x_n;

	vector< Vector3D >* result = new vector< Vector3D >;

	//1. 점들 채우기
	for (int i = 0; i <= n; i++) {
		for (int j = 0; j <= n; j++) {
			tmp_z_0.push_back( Vector3D((double)j / (double)n, (double)i/(double)n, 0.0)); // (0,0,0) ~ (n,n,0)
			tmp_z_n.push_back( Vector3D((double)j/ (double)n, (double)i/ (double)n, 1.0 ) ); // (0,0,n) ~ (n,n,n)

			tmp_x_0.push_back( Vector3D(0.0, (double)j/ (double)n, (double)i/ (double)n) ) ;
			tmp_x_n.push_back( Vector3D(1.0, (double)j/ (double)n, (double)i/ (double)n) );
		}
	}

	//tmp_z_0, tmp_z_n을 하나씩 고대로 push_back
	for (int i = 0; i < tmp_z_0.size(); i++) {
		result->push_back(tmp_z_0[i]);
		result->push_back(tmp_z_n[i]);
	}

	//tmp_x_0, tmp_x_n을 하나씩 고대로 push_back
	for (int i = 0; i < tmp_x_0.size(); i++) {
		result->push_back(tmp_x_0[i]);
		result->push_back(tmp_x_n[i]);
	}

	return result;
}