#pragma once

#include "myVector2D.h"
#include "particle.h"
#include "grid.h"
#include <vector>
#include <math.h>
#include <iostream>
#include <Eigen/SparseCore>
#include <Eigen/IterativeLinearSolvers>
#include <Eigen/Dense>
#include <random>
#include <algorithm>

//���� ����
#include<cstdlib> //rand(), srand()
#include<ctime>


using namespace std;

class Constant_Acceleration_Simulator {

public:

    //particle���� ���� vector
    vector<Particle2D> particles;
    float timestep;

    MAC_Grid<Vector2D> velocity_grid;


    //�ƹ� �Է°��� ���ٸ� �⺻ particle ���� 100��, grid_N = 10
    Constant_Acceleration_Simulator() {

        //���ڵ� �ʱ�ȭ
        srand((unsigned int)time(NULL));
        for (int i = 0; i < 100; i++) {
            //0~99 ���� ����    
            int randomLocation_X = rand() % 100;
            //0~49 ���� ����
            int randomLocation_Y = rand() % 50;

            //��ġ X : 0.0~ 0.99, ��ġ Y : 0.5 ~ 0.99
            Particle2D tmp = Particle2D((float)randomLocation_X / 100.0, (float)randomLocation_Y / 100.0 + 0.5, 0, 0, 0, -0.0098);
            particles.push_back(tmp);
        }

        //velocity grid �׸���
        velocity_grid = MAC_Grid<Vector2D>(10);

        //timestep�� default = 0.06
        timestep = 0.06;
    }

    Constant_Acceleration_Simulator(int particle_number, int grid_N) {

        //���ڵ� �ʱ�ȭ
        srand((unsigned int)time(NULL));
        for (int i = 0; i < particle_number; i++) {
            //0~99 ���� ����    
            int randomLocation_X = rand() % 100;
            //0~49 ���� ����
            int randomLocation_Y = rand() % 50;

            //��ġ X : 0.0~ 0.99, ��ġ Y : 0.5 ~ 0.99
            Particle2D tmp = Particle2D((float)randomLocation_X / 100.0, (float)randomLocation_Y / 100.0 + 0.5, 0, 0, 0, -0.098);
            particles.push_back(tmp);
        }

        //velocity grid �׸���
        velocity_grid = MAC_Grid<Vector2D>(grid_N);

        //timestep�� default = 0.06
        timestep = 0.06;
    }

    void particle_simulation() {
        //�ӵ� update
        Update_particles_Velocity();

        //��ġ update
        Update_particles_Location();

    }


    //���ڵ��� �ӵ��� ��� update
    void Update_particles_Velocity() {
        for (int i = 0; i < particles.size(); i++) {
            this->particles[i].Update_particle_Velocity(timestep);
        }
    }

    //���ڵ��� ��ġ�� ��� update
    void Update_particles_Location() {
        for (int i = 0; i < particles.size(); i++) {
            this->particles[i].Update_particle_Location(timestep);
        }
    }



};