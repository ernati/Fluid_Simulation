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

class Fluid_Simulator_OnlyParticle {
public:

    //particle���� ���� vector
    vector<Particle2D> particles;
    float timestep;
    float density;

    float delta_x;
    float delta_y;

    //particle���� �ӵ��� ���� vector
    vector<float> previous_velocity_x;
    vector<float> previous_velocity_y;

    vector<float> next_velocity_x;
    vector<float> next_velocity_y;

    //�ƹ� �Է°��� ���ٸ� �⺻ particle ���� 100��, grid_N = 10
    Fluid_Simulator_OnlyParticle() {

        //���ڵ� �ʱ�ȭ
        srand((unsigned int)time(NULL));
        for (int i = 0; i < 100; i++) {
            //0~99 ���� ����    
            int randomLocation_X = rand() % 100;
            //0~99 ���� ����
            int randomLocation_Y = rand() % 100;

            //��ġ X : 0.1~ 0.2, ��ġ Y : 0.8 ~ 0.9
            Particle2D tmp = Particle2D(((float)randomLocation_X + 100.0) / 1000.0, ((float)randomLocation_X + 800.0) / 1000.0, (float)randomLocation_X / 500.0, (float)randomLocation_Y / 500.0, 0, -0.98);
            particles.push_back(tmp);

            previous_velocity_x.push_back(tmp.Velocity.X);
            previous_velocity_y.push_back(tmp.Velocity.Y);

            next_velocity_x.push_back(0.0);
            next_velocity_y.push_back(0.0);
        }

        //timestep�� default = 0.06
        timestep = 0.06;
        density = 1.0;
        delta_x = 1.0 / 10.0;
        delta_y = 1.0 / 10.0;
    }

    Fluid_Simulator_OnlyParticle(int particle_number, int grid_N) {

        //���ڵ� �ʱ�ȭ
        srand((unsigned int)time(NULL));
        for (int i = 0; i < particle_number; i++) {
            //0~99 ���� ����    
            int randomLocation_X = rand() % 100;
            //0~99 ���� ����
            int randomLocation_Y = rand() % 100;

            //��ġ X : 0.1~ 0.2, ��ġ Y : 0.8 ~ 0.9
            Particle2D tmp = Particle2D(((float)randomLocation_X + 100.0) / 1000.0, ((float)randomLocation_X + 800.0) / 1000.0, (float)randomLocation_X / 500.0 + 0.1, (float)randomLocation_Y / 500.0, 0, -0.98);
            particles.push_back(tmp);

            previous_velocity_x.push_back(tmp.Velocity.X);
            previous_velocity_y.push_back(tmp.Velocity.Y);

            next_velocity_x.push_back(0.0);
            next_velocity_y.push_back(0.0);
        }

        //timestep�� default = 0.06
        timestep = 0.06;
        density = 1.0;
        delta_x = 1.0 / (float)grid_N;
        delta_y = 1.0 / (float)grid_N;
    }

    void particle_simulation() {

        cout << " boundarycondition start " << endl;

        //1. ��� ó��
        boundarycondition();

        cout << " advection start " << endl;

        //2. advection
        advection();

        cout << " bodyforce start " << endl;

        //3. bodyforce ����
        bodyforce();

        cout << " Adjust_velocity_from_bodyforce start " << endl;

        //3. bodyforce ����
        Adjust_velocity_from_bodyforce();

        cout << " pressureSolve start " << endl;

        //4. �з� ���
        pressure_solve(1.0);

        //5. �߰����� ��ȣ�ۿ�/
        //...

        cout << " Update particle Velocity start " << endl;

        //6. ��ȣ �ۿ뿡 ���� �ӵ� ���
        Update_particles_Velocity();

        cout << " Swap buffers start " << endl;

        //7. new�� previous �ڸ��� �ٲ۴�.
        swap_buffers();
    }

    //�ӵ��� ���� ��ġ ���� ������� boundarycondition�� ����� ��� �ӵ��� ������ �������� ������.
    void boundarycondition() {
        for (int i = 0; i < particles.size(); i++) {
            Vector2D expecting_Location = this->particles[i].Location + this->particles[i].Velocity * timestep;

            //boundary�� �����, particle�� ��ġ�� �ű��, �ӵ��� 0���� �ʱ�ȭ.
            if (!check_location_for_boundary(expecting_Location)) { boundary_work(particles[i]); }
        }
    }

    bool check_location_for_boundary(Vector2D location) {
        if (location.X < 0) { return false; }
        if (location.X > 1) { return false; }
        if (location.Y < 0) { return false; }
        if (location.Y > 1) { return false; }

        return true;
    }

    void boundary_work(Particle2D& particle) {
        //particle.Location.X = 0.5;
        particle.Velocity.X = 0.0;

        particle.Location.Y = 0.1;
        particle.Velocity.Y = 0.0;
    }

    void advection() {
        for (int i = 0; i < particles.size(); i++) {
            this->particles[i].Location = this->particles[i].Location + this->particles[i].Velocity * timestep;
        }
    }

    //particle�� �ӵ��� previous vector�� �ű�
    void Update_Velocity_particle_to_vector() {
        for (int i = 0; i < particles.size(); i++) {
            previous_velocity_x[i] = particles[i].Velocity.X;
            previous_velocity_y[i] = particles[i].Velocity.Y;
        }
    }

    //next vector�� �ӵ��� particle�� �ű�
    void Update_Velocity_vector_to_particle() {
        for (int i = 0; i < particles.size(); i++) {
            Vector2D vel = Vector2D(next_velocity_x[i], next_velocity_y[i]);
            particles[i].Velocity = vel;
        }
    }


    //���ڵ��� �ӵ��� ��� update
    void Update_particles_Velocity() {
        //1. vector�� �ӵ��� particle�� �ű�
        Update_Velocity_vector_to_particle();
    }

    void bodyforce() {
        //1. particle�� ���ӵ� ����
        for (int i = 0; i < particles.size(); i++) {
            this->particles[i].Acceleration.X = 0.0;
            this->particles[i].Acceleration.Y = -0.98;
        }

    }

    void Adjust_velocity_from_bodyforce() {
        //1. ���ӵ��� particle�� �ӵ� ��� -> u = u + tg
        for (int i = 0; i < previous_velocity_y.size(); i++) {
            previous_velocity_y[i] = previous_velocity_y[i] + timestep * this->particles[i].Acceleration.Y;
        }
    }

    void pressure_solve(float threshold_distance) {


        //0. �з� ��� �� ��� ����
        float pressure_coefficient = timestep / density;

        int n_particles = particles.size();

        Eigen::VectorXd x(n_particles), b(n_particles);

        Eigen::SparseMatrix<double> A(n_particles, n_particles);

        //solver
        Eigen::BiCGSTAB<Eigen::SparseMatrix<double>> cg_solver;

        //1. Ax=b���� A �ۼ� - ���� ���� �Ÿ� ����
        for (int i = 0; i < particles.size(); i++) {
            for (int j = 0; j < particles.size(); j++) {
                // 1. ( i,j )�� ��� 
                if (i < j) {
                    // 1-1. �Ÿ� ����
                    float distance = particles[i].get_distance_from_point(particles[j].Location);

                    // 1-2. ��� �ۼ�, �ݴ��� ��ġ�� �ۼ�
                    if (distance > threshold_distance) { A.insert(i, j) = 0.0; A.insert(j, i) = 0.0; }
                    else { A.insert(i, j) = pressure_coefficient / (distance * distance);  A.insert(j, i) = pressure_coefficient / (distance * distance); }

                }
            }
        }

        //diagonal ��ҵ鵵 �ۼ�
        for (int i = 0; i < particles.size(); i++) {
            A.insert(i, i) = 1.0;
        }

        //2. Ax=b���� b �ۼ� - ������ �߻� - �׳� ��Ҹ� �� ������.
        for (int i = 0; i < particles.size(); i++) {
            b(i) = particles[i].Velocity.X + particles[i].Velocity.Y;
        }

        //3. x solve
        cg_solver.compute(A);
        x = cg_solver.solve(b);

        std::cout << "#iterations:     " << cg_solver.iterations() << std::endl;
        std::cout << "estimated error: " << cg_solver.error() << std::endl;

        ////cout << A << endl;
        //cout << "===========================================" << endl;
        //cout << x << endl;
        //cout << "===========================================" << endl;
        //cout << b << endl;

        ///* ... update b ... */
        ////x = cg_solver.solve(b); // solve again

        ////cout << A << endl;
        //cout << "===========================================" << endl;
        //cout << x << endl;
        //cout << "===========================================" << endl;
        //cout << b << endl;

        cout << " solver_done " << endl;

        //4. x ��( �з� )�� ���� ���ڵ��� particle ���
        for (int i = 0; i < previous_velocity_x.size(); i++) {
            //4-1. ���ο� �ӵ� ��� - �з� �״�� ���
            float new_vel_x = previous_velocity_x[i] - timestep / density * x(i);
            float new_vel_y = previous_velocity_y[i] - timestep / density * x(i);

            //4-2. new vel���ٰ� �Է��ϱ�
            next_velocity_x[i] = new_vel_x;
            next_velocity_y[i] = new_vel_y;
        }

    }

    void swap_buffers() {
        vector<float> tmp_x;
        vector<float> tmp_y;

        //1. new�� tmp�� ��� ����
        for (int i = 0; i < next_velocity_x.size(); i++) {
            tmp_x.push_back(next_velocity_x[i]);
            tmp_y.push_back(next_velocity_y[i]);
        }

        //2. new�� previous�� ��� ����
        for (int i = 0; i < previous_velocity_x.size(); i++) {
            next_velocity_x[i] = previous_velocity_x[i];
            next_velocity_y[i] = previous_velocity_y[i];
        }

        //3. previous�� tmp�� ��´�.
        for (int i = 0; i < tmp_x.size(); i++) {
            previous_velocity_x[i] = tmp_x[i];
            previous_velocity_y[i] = tmp_y[i];
        }
    }

};