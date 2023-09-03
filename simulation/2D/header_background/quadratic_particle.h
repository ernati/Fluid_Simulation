#pragma once
#include <vgl.h>
#include <InitShader.h>
#include <mat.h>
#include <cstdio>
#include <cstdlib>

#include "2D_particle_simulation.h"


// coefficient for -2( t - 999/2 )
struct coefficient {
	int a; //-2.0
	int b; // 999.0/2.0

};


//
class quadratic_solver {
public :

	coefficient coef;

	quadratic_solver() {
		coef.a = -2.0;
		coef.b = 999.0 / 2.0;
	}

	~quadratic_solver() {

	}

	//calculate -2(t - 500)
	double calculate(int t) {
		return coef.a * (t - coef.b);
	}

	double get_a() {
		return coef.a;
	}

	double get_b() {
		return coef.b;
	}


private :
	/*coefficient coef;*/
};

class quadratic_particle : public Particle {
public:

	quadratic_solver solver;

	quadratic_particle() {

	}

	quadratic_particle(vec2 sca, vec2 Loc, vec2 vel) {
		Scale = sca;
		Location = Loc;
		Velocity = vel;
		solver = quadratic_solver();
	}

	// ��ġ = ���� ��ġ + ���� �ӵ�
	void move(int t_1) {

		//�ӵ� ���
		Velocity.y = double(solver.calculate( t_1% 1000 )) / 1000000.0;

		if (0 < t_1 % 1000 < 500) {
			Velocity.y = Velocity.y / 2;
		}

		else {
			Velocity.y = Velocity.y * 2;
		}


		// ���� ��ġ ���
		double next_Loc_x = Location.x + Velocity.x;
		double next_Loc_y = Location.y + Velocity.y;

		//boundary ����
		// �� ��ġ ��꿡 ���� �״�� ��������, �ӵ��� �ݴ�� �������� ���
		if (next_Loc_x < 0.0 || next_Loc_x > 1.0) {
			Velocity.x = -1 * Velocity.x;
		}

		if (next_Loc_y < 0.0 || next_Loc_y > 1.0) {
			Velocity.y = -1 * Velocity.y;
		}

		Location = Location + Velocity;
	}


private:
	/*quadratic_solver solver;*/
};