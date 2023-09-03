#pragma once
#include <vgl.h>
#include <InitShader.h>
#include <mat.h>
#include <cstdio>
#include <cstdlib>


class Particle {
public:
	vec2 Scale;
	vec2 Location;
	vec2 Velocity;

	Particle()
	{

	}

	Particle(vec2 a, vec2 b, vec2 c) {
		Scale = a;
		Location = b;
		Velocity = c;
	}

	~Particle()
	{

	}

	// ��ġ = ���� ��ġ + ���� �ӵ�
	void move() {
		// ���� ��ġ ���
		float next_Loc_x = Location.x + Velocity.x;
		float next_Loc_y = Location.y + Velocity.y;

		// �� ��ġ ��꿡 ���� �״�� ��������, �ӵ��� �ݴ�� �������� ���
		if (next_Loc_x < 0.0 || next_Loc_x > 1.0) {
			Velocity.x = -1 * Velocity.x;
		}

		if (next_Loc_y < 0.0 || next_Loc_y > 1.0) {
			Velocity.y = -1 * Velocity.y;
		}

		Location = Location + Velocity;


			
	}

};

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

class Simulation {
public:
	
};