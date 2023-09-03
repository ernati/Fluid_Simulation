#pragma once
#include <vector>
#include "particle3D.h"

using namespace std;

class Box3D {
public:
	double xmin;
	double xmax;
	double ymin;
	double ymax;
	double zmin;
	double zmax;

	Box3D() {

	}

	Box3D(double a, double b, double c, double d,double e, double f) {
		xmin = a; xmax = b; ymin = c; ymax = d; zmin = e; zmax = f;
	}

	~Box3D() {

	}

	vector<Vector3D>* make_box_points() {
		vector<Vector3D>* result = new vector<Vector3D>;

		//정사각형 1
		result->push_back(Vector3D(xmin, ymin, zmin));
		result->push_back(Vector3D(xmax, ymin, zmin));
		result->push_back(Vector3D(xmax, ymax, zmin));
		result->push_back(Vector3D(xmin, ymax, zmin));

		//정사각형 2
		result->push_back(Vector3D(xmin, ymin, zmax));
		result->push_back(Vector3D(xmax, ymin, zmax));
		result->push_back(Vector3D(xmax, ymax, zmax));
		result->push_back(Vector3D(xmin, ymax, zmax));

		//정사각형 3
		result->push_back(Vector3D(xmin, ymin, zmin));
		result->push_back(Vector3D(xmin, ymin, zmax));
		result->push_back(Vector3D(xmin, ymax, zmax));
		result->push_back(Vector3D(xmin, ymax, zmin));

		//정사각형 4
		result->push_back(Vector3D(xmax, ymin, zmin));
		result->push_back(Vector3D(xmax, ymin, zmax));
		result->push_back(Vector3D(xmax, ymax, zmax));
		result->push_back(Vector3D(xmax, ymax, zmin));

		return result;
	}
};