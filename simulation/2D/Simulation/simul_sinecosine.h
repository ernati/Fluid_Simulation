#pragma once

#include "../header_background/myVector2D.h"
#include "../header_background/particle.h"
#include "../header_background/grid.h"
#include <vector>
#include <cmath>
#include <iostream>
#include <Eigen/SparseCore>
#include <Eigen/IterativeLinearSolvers>
#include <Eigen/Dense>
#include <random>
#include <algorithm>
#include <thread>
#include <chrono>

//난수 생성
#include<cstdlib> //rand(), srand()
#include<ctime>


class Simul_SineCosine {
public:

	int particle_num;
	vector<Vector2D>* sine_particles;
	vector<Vector2D>* cosine_particles;

	double timestep;
	double accumulate_timestep;

	double pi = 3.141592;



	Simul_SineCosine();
	Simul_SineCosine(int number);
	void particle_simulation();
	void accumulation_time();
	void calculate_sine();
	void calculate_cosine();

	//multithread
	void calculate_sine_thread(int start, int end);
	void calculate_cosine_thread(int start, int end);
};

Simul_SineCosine::Simul_SineCosine() {
	;
}

Simul_SineCosine::Simul_SineCosine(int number) {

	sine_particles = new vector<Vector2D>;
	cosine_particles = new vector<Vector2D>;

	particle_num = number;
	timestep = 0.06;

	accumulate_timestep = 0.0;

	for (int i = 0; i < number; i++) {
		sine_particles->push_back(Vector2D( 1.0 / number * i , 0.0));
		cosine_particles->push_back(Vector2D(1.0 / number * i, 0.0));
	}
}

void Simul_SineCosine::accumulation_time() {
	accumulate_timestep += timestep;
}

//  4/10 * Sine( 2pi *x + 0.12 * pi )
void Simul_SineCosine::calculate_sine() {
	for (int i = 0; i < sine_particles->size(); i++) {
		(*sine_particles)[i].Y = 4.0 / 10.0 * sin(2.0 * pi * (*sine_particles)[i].X + 2.0 * accumulate_timestep * pi) + 0.5 ;
	}
}

//  4/10 * Cosine( 2pi *x + 0.12 * pi )
void Simul_SineCosine::calculate_cosine() {
	for (int i = 0; i < sine_particles->size(); i++) {
		(*cosine_particles)[i].Y = 4.0 / 10.0 * cos(2.0 * pi * (*cosine_particles)[i].X + 2.0 * accumulate_timestep * pi) + 0.5;
	}
}

void Simul_SineCosine::particle_simulation() {
	
	accumulation_time();

	calculate_sine();
	calculate_cosine();

}


//  4/10 * Sine( 2pi *x + 0.12 * pi )
void Simul_SineCosine::calculate_sine_thread(int start, int end) {
	for (int i = start; i < end; i++) {
		(*sine_particles)[i].Y = 4.0 / 10.0 * sin(2.0 * pi * (*sine_particles)[i].X + 2.0 * accumulate_timestep * pi) + 0.5;
	}
}

//  4/10 * Cosine( 2pi *x + 0.12 * pi )
void Simul_SineCosine::calculate_cosine_thread( int start, int end ) {
	for (int i = start; i < end; i++) {
		(*cosine_particles)[i].Y = 4.0 / 10.0 * cos(2.0 * pi * (*cosine_particles)[i].X + 2.0 * accumulate_timestep * pi) + 0.5;
	}
}