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

class Constant_Acceleration_Simulation_2D {

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


