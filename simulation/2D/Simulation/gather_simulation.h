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
	//중심의 범위가 될 반지름
	double center_radius;

	//입자들이 모일 center 좌표
	Vector2D center;
	//particle들을 담을 vector
	vector<Particle2D> particles;

	//기본생성자
	GatherSimulation();
	//입자 수를 인자로 받는 생성자
	GatherSimulation(int number);

	//simulation
	//시뮬레이션 함수
	void particle_simulation();

	//입자들의 가속도를 업데이트하는 함수
	void Update_Acceleration();

	//입자들의 속도를 업데이트하는 함수
	void Update_Velocity();

	//입자들의 위치를 업데이트하는 함수
	void Update_Location();

	//입자가 boundary를 넘어갔을 때를 체크하는 함수
	bool check_location_for_boundary(Vector2D& location);

	//입자가 boundary 체크일 때 처리 함수
	void boundary_work( Particle2D& particle );

	void clear_and_ReInit();
	void clear();

	//multithread
	//1,2,3,4,... 는 한 함수를 여러스레드에 분할 할당하기 위해 반복문을 나눈 함수들이다.
	//미리 결과를 말하자면, 이렇게 잘게 분할하는 것은, 컨텍스트 스위치의 오버헤드가 너무 커서 오히려 속도가 느려졌다.
	void Update_Acceleration_thread(int start, int end);
	void Update_Velocity_thread(int start, int end);
	void Update_Location_thread(int start, int end);
};

//기본생성자
GatherSimulation::GatherSimulation()
{
	;
}

//입자 수를 인자로 받는 생성자
GatherSimulation::GatherSimulation(int number) {
	timestep = 0.06;
	center_radius = 0.05;
	center = Vector2D(0.5, 0.5);

	srand((unsigned int)time(NULL));
	//xmin - box의 xmin쪽에 가까운 좌표들을 랜덤하게 생성
	for (int i = 0; i < number / 4; i++) {
		int randomLocation = rand() % 400 - 200; // -200 ~ 200
		int randomLocation2 = rand() % (number / 4);

		particles.push_back( Particle2D(  static_cast<double>( randomLocation2 ) / (number / 4)  , 1.0 / static_cast<double>(randomLocation), 0.0, 0.0, 0.0, 0.0) ) ;
	}

	//xmax - box의 xmax쪽에 가까운 좌표들을 랜덤하게 생성 
	for (int i = number/4; i < number / 4 * 2; i++) {
		int randomLocation = rand() % 400 - 200; // -200 ~ 200
		int randomLocation2 = rand() % (number / 4);

		particles.push_back( Particle2D(  static_cast<double>( randomLocation2 ) / (number / 4)  , 1.0 + 1.0 / static_cast<double>(randomLocation), 0.0, 0.0, 0.0, 0.0) ) ;
	}

	//ymin - box의 ymin쪽에 가까운 좌표들을 랜덤하게 생성 
	for (int i = number/4*2; i < number / 4 * 3; i++) {
		int randomLocation = rand() % 400 - 200; // -200 ~ 200
		int randomLocation2 = rand() % (number / 4);

		particles.push_back( Particle2D( 1.0 / static_cast<double>( randomLocation), static_cast<double>(randomLocation2) / (number / 4) , 0.0, 0.0, 0.0, 0.0) ) ;
	}

	//ymax - box의 ymax쪽에 가까운 좌표들을 랜덤하게 생성
	for (int i = number / 4 * 3; i < number; i++) {
		int randomLocation = rand() % 400 - 200; // -200 ~ 200
		int randomLocation2 = rand() % (number / 4);

		particles.push_back( Particle2D(  1.0 + 1.0 / static_cast<double>( randomLocation), static_cast<double>(randomLocation2) / (number / 4) , 0.0, 0.0, 0.0, 0.0) ) ;
	}
}

void GatherSimulation::clear_and_ReInit() {
	particles.clear();
	GatherSimulation( particles.size() );
}

//입자들의 가속도를 업데이트하는 함수
void GatherSimulation::Update_Acceleration()
{
	for (int i = 0; i < particles.size(); i++)
	{
		Vector2D tmp = Vector2D(center.X - particles[i].Location.X, center.Y - particles[i].Location.Y);
		particles[i].Acceleration = tmp;
	}
}

//입자들의 속도를 업데이트하는 함수
void GatherSimulation::Update_Velocity() {
	for (int i = 0; i < particles.size(); i++) {
		particles[i].Velocity = particles[i].Velocity + particles[i].Acceleration * timestep;
	}
}

//입자들의 위치를 업데이트하는 함수
void GatherSimulation::Update_Location() {
	for (int i = 0; i < particles.size(); i++) {
		particles[i].Location = particles[i].Location + particles[i].Velocity * timestep;

		//boundary condition
		if (check_location_for_boundary(particles[i].Location)) {
			boundary_work( particles[i] );
		}

	}
}

//입자가 boundary를 넘어갔을 때를 체크하는 함수
//입자와 center의 거리와 반지름을 비교
bool GatherSimulation::check_location_for_boundary(Vector2D& location) {
	double distance = sqrt(pow(location.X - center.X, 2) + pow(location.Y - center.Y, 2));
	if (distance < center_radius) { return true; }
	else { return false; }
}




//boundary condition
//입자가 boundary 체크일 때 처리 함수
//입자가 중심 좌표 근처로 가면, 중심좌표 -> 입자 방향벡터를 구한 후, 벡터의 크기를 1로 늘린 후, 충돌한 시점의 입자에 위치에 이 벡터를 더해준다.
//그러면, 입자는 중심 -> 원래 위치 방향벡터를 따라 연장선의 길이가 1인 지점으로 옮겨지게 된다.
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

//시뮬레이션 함수
void GatherSimulation::particle_simulation() {

	//가속도를 update합니다.
	Update_Acceleration();
	//속도를 update합니다
	Update_Velocity();
	//위치를 update합니다.
	Update_Location();

}

//=============================================multithread===============================================================

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
