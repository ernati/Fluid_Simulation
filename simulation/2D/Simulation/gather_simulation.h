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


class GatherSimulation
{
public:
	double timestep;
	double center_radius;
	Vector2D center;
	vector<Particle2D> particles;

	GatherSimulation();
	GatherSimulation(int number);

	//simulation
	void particle_simulation();
	void Update_Acceleration();
	void Update_Velocity();
	void Update_Location();
	bool check_location_for_boundary(Vector2D& location);
	void boundary_work( Particle2D& particle );

	//multithread
	void Update_Acceleration_thread(int start, int end);
	void Update_Velocity_thread(int start, int end);
	void Update_Location_thread(int start, int end);
};


GatherSimulation::GatherSimulation()
{
	;
}

GatherSimulation::GatherSimulation(int number) {
	timestep = 0.06;
	center_radius = 0.05;
	center = Vector2D(0.5, 0.5);

	srand((unsigned int)time(NULL));
	//xmin
	for (int i = 0; i < number / 4; i++) {
		int randomLocation = rand() % 400 - 200; // -200 ~ 200
		int randomLocation2 = rand() % (number / 4);

		particles.push_back( Particle2D(  static_cast<double>( randomLocation2 ) / (number / 4)  , 1.0 / static_cast<double>(randomLocation), 0.0, 0.0, 0.0, 0.0) ) ;
	}

	//xmax
	for (int i = number/4; i < number / 4 * 2; i++) {
		int randomLocation = rand() % 400 - 200; // -200 ~ 200
		int randomLocation2 = rand() % (number / 4);

		particles.push_back( Particle2D(  static_cast<double>( randomLocation2 ) / (number / 4)  , 1.0 + 1.0 / static_cast<double>(randomLocation), 0.0, 0.0, 0.0, 0.0) ) ;
	}

	//ymin
	for (int i = number/4*2; i < number / 4 * 3; i++) {
		int randomLocation = rand() % 400 - 200; // -200 ~ 200
		int randomLocation2 = rand() % (number / 4);

		particles.push_back( Particle2D( 1.0 / static_cast<double>( randomLocation), static_cast<double>(randomLocation2) / (number / 4) , 0.0, 0.0, 0.0, 0.0) ) ;
	}

	//ymax
	for (int i = number / 4 * 3; i < number; i++) {
		int randomLocation = rand() % 400 - 200; // -200 ~ 200
		int randomLocation2 = rand() % (number / 4);

		particles.push_back( Particle2D(  1.0 + 1.0 / static_cast<double>( randomLocation), static_cast<double>(randomLocation2) / (number / 4) , 0.0, 0.0, 0.0, 0.0) ) ;
	}
}

void GatherSimulation::Update_Acceleration()
{
	for (int i = 0; i < particles.size(); i++)
	{
		Vector2D tmp = Vector2D(center.X - particles[i].Location.X, center.Y - particles[i].Location.Y);
		particles[i].Acceleration = tmp;
	}
}

void GatherSimulation::Update_Velocity() {
	for (int i = 0; i < particles.size(); i++) {
		particles[i].Velocity = particles[i].Velocity + particles[i].Acceleration * timestep;
	}
}

void GatherSimulation::Update_Location() {
	for (int i = 0; i < particles.size(); i++) {
		particles[i].Location = particles[i].Location + particles[i].Velocity * timestep;

		//boundary condition
		if (check_location_for_boundary(particles[i].Location)) {
			boundary_work( particles[i] );
		}

	}
}

bool GatherSimulation::check_location_for_boundary(Vector2D& location) {
	double distance = sqrt(pow(location.X - center.X, 2) + pow(location.Y - center.Y, 2));
	if (distance < center_radius) { return true; }
	else { return false; }
}




//boundary condition
void GatherSimulation::boundary_work( Particle2D& particle ) {
	Vector2D vector_C_to_A = Vector2D(particle.Location.X - center.X, particle.Location.Y - center.Y);

	//normalization
	double size = sqrt(pow(vector_C_to_A.X, 2) + pow(vector_C_to_A.Y, 2)) * 2.0;
	vector_C_to_A = vector_C_to_A / size;

	//transformation
	vector_C_to_A = vector_C_to_A + center;

	particle.Location.X = vector_C_to_A.X;
	particle.Location.Y = vector_C_to_A.Y;

	//Velocity to zero
	particle.Velocity.X = 0.0;
	particle.Velocity.Y = 0.0;
}

void GatherSimulation::particle_simulation() {

	Update_Acceleration();
	Update_Velocity();
	Update_Location();

}


void GatherSimulation::Update_Acceleration_thread(int start, int end) {
	for (int i = start; i < end; i++)
	{
		Vector2D tmp = Vector2D(center.X - particles[i].Location.X, center.Y - particles[i].Location.Y);
		particles[i].Acceleration = tmp;
	}
}


void GatherSimulation::Update_Velocity_thread(int start, int end) {
	for (int i = start; i < end; i++) {
		particles[i].Velocity = particles[i].Velocity + particles[i].Acceleration * timestep;
	}
}

void GatherSimulation::Update_Location_thread(int start, int end) {
	for (int i = start; i < end; i++) {
		particles[i].Location = particles[i].Location + particles[i].Velocity * timestep;

		//boundary condition
		if (check_location_for_boundary(particles[i].Location)) {
			boundary_work( particles[i] );
		}

	}
}