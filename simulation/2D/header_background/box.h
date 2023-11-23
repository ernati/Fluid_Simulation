#pragma once

//box를 나타내는 x,y 최소 최대 좌표를 담는 클래스
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
