#pragma once

class Box {

public:
	float xmin;
	float xmax;
	float ymin;
	float ymax;

	Box() {

	}

	Box(float x1, float x2, float y1, float y2) {
		xmin = x1; xmax = x2; ymin = y1; ymax = y2;
	}
};