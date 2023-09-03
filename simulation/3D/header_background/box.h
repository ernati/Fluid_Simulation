#pragma once


class Box {
public:
	float xmin;
	float xmax;
	float ymin;
	float ymax;

	Box() {

	}

	Box(float a, float b, float c, float d) {
		xmin = a; xmax = b; ymin = c; ymax = d;
	}

	~Box() {

	}
};