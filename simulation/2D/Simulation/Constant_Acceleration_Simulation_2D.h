#pragma once

#include "../header_background/myVector2D.h"
#include "../header_background/particle.h"
#include "../header_background/grid.h"
#include <vector>
#include <math.h>
#include <iostream>
#include <Eigen/SparseCore>
#include <Eigen/IterativeLinearSolvers>
#include <Eigen/Dense>
#include <random>
#include <algorithm>

//난수 생성
#include<cstdlib> //rand(), srand()
#include<ctime>


using namespace std;

class Constant_Acceleration_Simulator {

public:

    //particle들을 담을 vector
    vector<Particle2D> particles;
    float timestep;

    MAC_Grid<Vector2D> velocity_grid;

    //기본 생성자
    Constant_Acceleration_Simulator();
    //입자 수와 grid의 크기를 입력받는 생성자
    Constant_Acceleration_Simulator(int particle_number, int grid_N);
    //simulation 함수
    void particle_simulation();
    //입자들의 속도를 모두 update
    void Update_particles_Velocity();
    //입자들의 위치를 모두 update
    void Update_particles_Location();

    //multithread
    void Update_particles_Velocity_Thread(int start, int end);
    void Update_particles_Location_Thread(int start, int end);
};


//아무 입력값이 없다면 기본 particle 수는 100개, grid_N = 10
Constant_Acceleration_Simulator::Constant_Acceleration_Simulator() {

    //입자들 초기화
    srand((unsigned int)time(NULL));
    for (int i = 0; i < 100; i++) {
        //0~99 난수 생성    
        int randomLocation_X = rand() % 100;
        //0~49 난수 생성
        int randomLocation_Y = rand() % 50;

        //위치 X : 0.0~ 0.99, 위치 Y : 0.5 ~ 0.99
        Particle2D tmp = Particle2D((float)randomLocation_X / 100.0, (float)randomLocation_Y / 100.0 + 0.5, 0, 0, 0, -0.0098);
        particles.push_back(tmp);
    }

    //velocity grid 그리기
    velocity_grid = MAC_Grid<Vector2D>(10);

    //timestep의 default = 0.06
    timestep = 0.06;
}

Constant_Acceleration_Simulator::Constant_Acceleration_Simulator(int particle_number, int grid_N) {

    //입자들 초기화
    srand((unsigned int)time(NULL));
    for (int i = 0; i < particle_number; i++) {
        //0~99 난수 생성    
        int randomLocation_X = rand() % 100;
        //0~49 난수 생성
        int randomLocation_Y = rand() % 50;

        //위치 X : 0.0~ 0.99, 위치 Y : 0.5 ~ 0.99
        Particle2D tmp = Particle2D((float)randomLocation_X / 100.0, (float)randomLocation_Y / 100.0 + 0.5, 0, 0, 0, -0.98);
        particles.push_back(tmp);
    }

    //velocity grid 그리기
    velocity_grid = MAC_Grid<Vector2D>(grid_N);

    //timestep의 default = 0.06
    timestep = 0.06;
}

void Constant_Acceleration_Simulator::particle_simulation() {
    //속도 update
    Update_particles_Velocity();

    //위치 update
    Update_particles_Location();

}


//입자들의 속도를 모두 update
void Constant_Acceleration_Simulator::Update_particles_Velocity() {
    for (int i = 0; i < particles.size(); i++) {
	//속도 = 속도 + timestep * 가속도 <- forward euler
        this->particles[i].Update_particle_Velocity(timestep);
    }
}

//입자들의 위치를 모두 update
void Constant_Acceleration_Simulator::Update_particles_Location() {
    for (int i = 0; i < particles.size(); i++) {
	//위치 = 위치 + timestep * 속도 <-forward euler
        this->particles[i].Update_particle_Location(timestep);

        if (particles[i].Location.Y < 0.0) {
            particles[i].Location.Y = 0.5;
            particles[i].Velocity.Y = 0;
        }
    }
}

//=============================multithread=============================
// 기존 함수의 for문을 start부터 end까지 할당
// 스레드의 개수에 따라 입자수를 n분의 1로 분배한다.

void Constant_Acceleration_Simulator::Update_particles_Velocity_Thread(int start, int end) {
    for (int i = start; i < end; i++) {
	this->particles[i].Update_particle_Velocity(timestep);
    }
}

void Constant_Acceleration_Simulator::Update_particles_Location_Thread(int start, int end) {
    for (int i = start; i < end; i++) {
		this->particles[i].Update_particle_Location(timestep);

        if (particles[i].Location.Y < 0.0) {
			particles[i].Location.Y = 0.5;
			particles[i].Velocity.Y = 0;
		}
	}
}
