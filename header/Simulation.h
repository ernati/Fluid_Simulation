#pragma once

#include "myVector2D.h"
#include "particle.h"
#include "grid.h"
#include <vector>

//���� ����
#include<cstdlib> //rand(), srand()
#include<ctime>

using namespace std;

//��ӵ� � �ùķ��̼� - v_n+1_ = v_n_ + a*t
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

class Fluid_Simulator {

public:

    //particle���� ���� vector
    vector<Particle2D> particles;
    float timestep;
    int gridsize;

    //grid �Ѻ��� ����
    float cellsize;

    //cell���� �ӵ��� ���� grid
    MAC_Grid<Vector2D> velocity_grid;

    //cell���� ���ӵ��� ���� grid - �ܷ�
    MAC_Grid<Vector2D> bodyforce;

    //cell���� �߽���ǥ�� ���� grid
    MAC_Grid<Vector2D> cell_center_point;

    //cell���� �ӵ��� ������ �ʿ��� weight���� ���� grid
    MAC_Grid<float> weight_velocity_grid;

    //cell���� particle�� ���� ���� grid
    MAC_Grid<int> the_number_of_particles_in_cell;


    Fluid_Simulator() {
        //timestep�� default = 0.06
        timestep = 0.000006;

        gridsize = 10;

        cellsize = 1.0 / (float)gridsize;

        //velocity grid �ʱ�ȭ
        velocity_grid = MAC_Grid<Vector2D>(gridsize);
        for (int n = 0; n < (gridsize) * (gridsize); n++) {
            velocity_grid.cell_values.push_back(Vector2D());
        }
    }

    Fluid_Simulator(int particle_number, int grid_N) {

        //timestep�� default = 0.06
        timestep = 0.06;

        gridsize = grid_N;

        cellsize = 1.0 / (float)gridsize;

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

        //velocity grid �ʱ�ȭ
        velocity_grid = MAC_Grid<Vector2D>(gridsize);
        for (int n = 0; n < (gridsize) * (gridsize); n++) {
            velocity_grid.cell_values.push_back(Vector2D());
        }

        //body force �ʱ�ȭ
        bodyforce = MAC_Grid<Vector2D>(gridsize);
        for (int n = 0; n < (gridsize) * (gridsize); n++) {
            bodyforce.cell_values.push_back(Vector2D());
        }

        //cell_center_grid �ʱ�ȭ
        cell_center_point = MAC_Grid<Vector2D>(gridsize);
        //cell�鿡�ٰ� cell_point���� ����
        //1. cell�� ������ gridsize�� ����
        for (int v = 0; v < (gridsize) * (gridsize); v++) {
            //2. (0,0) �� ( 1/gridsize , 1/ gridsize )
            if (v == 0) {
                cell_center_point.cell_values.push_back(Vector2D(cellsize / 2.0, cellsize / 2.0));
            }
            //3. (i,j)�� ( 1/gridsize + i * gridsize , 1/gridsize + j* gridsize )
            else {
                //3-1. v ( vector Index ) to (i,j)
                int i = cell_center_point.get_cell_i_from_VectorIndex(v);
                int j = cell_center_point.get_cell_j_from_VectorIndex(v);

                //3-2. �� ����
                cell_center_point.cell_values.push_back(Vector2D(cellsize / 2.0 + i * cellsize, cellsize / 2.0 + j * cellsize));
            }
        }

        //weight grid �ʱ�ȭ
        weight_velocity_grid = MAC_Grid<float>(gridsize);
        for (int n = 0; n < (gridsize) * (gridsize); n++) {
            weight_velocity_grid.cell_values.push_back(0.0);
        }

        //cell���� particle�� ���� ���� grid
        the_number_of_particles_in_cell = MAC_Grid<int>(gridsize);
        for (int n = 0; n < (gridsize) * (gridsize); n++) {
            the_number_of_particles_in_cell.cell_values.push_back( 0 );
        }



    }

    void particle_simulation() {

        //particle to grid
        transfer_velocity_to_grid_from_particle();

        //grid�� bodyforce �߰�
        add_body_force();

        //bodyforce�� velocity�� ����
        apply_bodyforce_to_Velocity();

        //grid to particle - Velocity
        transfer_Velocity_to_particle_from_grid();

        //grid to particle - bodyforce(Acceleration) - 
        transfer_bodyforce_to_particle_from_grid();

        //Update particle Location
        Update_particles_Location();

    }

    //bodyforce�� �����ϰ� ���� �ܷ� �߰� - �߷�
    void add_body_force() {
        for (int i = 0; i < (gridsize * gridsize); i++) {
            bodyforce.cell_values[i] = Vector2D(0.0, -0.0000098);
        }
    }

    //bodyforce�� velocity grid�� ���� - ��ӵ�
    void apply_bodyforce_to_Velocity() {
        for (int i = 0; i < (gridsize * gridsize); i++) {
            velocity_grid.cell_values[i] = velocity_grid.cell_values[i] + bodyforce.cell_values[i] * timestep;
        }
    }

    //�ϰ������� ���� ���� ���޹޾Ƶ� ������, �ϴ� ���Ե� cell�� ������ ���� �� �ְ� �س���.
    void transfer_bodyforce_to_particle_from_grid() {
        for (int p = 0; p < particles.size(); p++) {
            //1. particle�� ��� cell���� �ľ�
            Vector2D i_j = bodyforce.get_cell_i_j_from_world(particles[p].Location);

            //2. bodyforce cell���� Accceleration�� ����
            particles[p].Acceleration = bodyforce.cell_values[bodyforce.get_VectorIndex_from_cell(i_j)];
        }
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

    //particle�� value�� ��� grid�� �����ϴ� �Լ�
    void transfer_velocity_to_grid_from_particle() {
        ////0. the number of particles in cell grid �ʱ�ȭ
        //for (int i = 0; i < gridsize * gridsize; i++) {
        //    the_number_of_particles_in_cell.cell_values[i] = 0;
        //}

        //for (int p = 0; p < particles.size(); p++) {
        //    //1. particle cell��ǥ ã��
        //    Vector2D i_j = velocity_grid.get_cell_i_j_from_world(particles[p].Location);

        //    //2. cell�� particle�� �� ���
        //    the_number_of_particles_in_cell.cell_values[the_number_of_particles_in_cell.get_VectorIndex_from_cell(i_j)]++;

        //    //3. velocity cell���� �ֺ� grid�� ����
        //    extrapolation_value_to_grid_from_particle(velocity_grid, particles[p].Location, velocity_grid.cell_values[velocity_grid.get_VectorIndex_from_cell(i_j)]);

        //}

        ////4. particle�� ����ŭ �ӵ����� ������.
        //for (int i = 0; i < gridsize * gridsize; i++) {
        //    if (the_number_of_particles_in_cell.cell_values[i] == 0) continue;
        //    velocity_grid.cell_values[i] = velocity_grid.cell_values[i] / (float)the_number_of_particles_in_cell.cell_values[i];
        //}

        //========================================

        for (int p = 0; p < particles.size(); p++) {
            //1. particle cell ��ǥ ã��
            Vector2D i_j = velocity_grid.get_cell_i_j_from_world(particles[p].Location);

            //2. particle�� cell�� ���� ��ǥ �Ÿ� ���ϱ�
            float weight = 1.0 / particles[p].get_distance_from_point(cell_center_point.cell_values[ cell_center_point.get_VectorIndex_from_cell( i_j ) ] );
            weight_velocity_grid.cell_values[weight_velocity_grid.cell_values[cell_center_point.get_VectorIndex_from_cell(i_j)]] = weight_velocity_grid.cell_values[weight_velocity_grid.cell_values[cell_center_point.get_VectorIndex_from_cell(i_j)]] + weight;

            //3. weight�� particle�� �ӵ��� ���ؼ� velocity_grid�� ���ϱ�
            velocity_grid.cell_values[velocity_grid.get_VectorIndex_from_cell(i_j)] = velocity_grid.cell_values[velocity_grid.get_VectorIndex_from_cell(i_j)] + velocity_grid.cell_values[velocity_grid.get_VectorIndex_from_cell(i_j)] * weight;
        }

        //4. weight������ ��� grid�� �����ֱ�
        for (int i = 0; i < gridsize * gridsize; i++) {
            if (weight_velocity_grid.cell_values[i] == 0) { continue; }
            velocity_grid.cell_values[i] = velocity_grid.cell_values[i] / weight_velocity_grid.cell_values[i];
        }

    }

    //particle�� ��ǥ�� ���� value���� �ֺ� grid�κ��� �޾ƿ��� �Լ�
    template <class T>
    T interpolate_value_to_particle_from_grid(MAC_Grid<T> grid, Vector2D particle) {
        //1. cell �ȿ����� particle�� ��ġã��
        //1-1. particle�� cell��ǥ ã��
        Vector2D i_j = grid.get_cell_i_j_from_world(particle);
        //1-2. particle�� cell�� ���ʸ�, �Ʒ������ �Ÿ� ���ϱ�
        float i_frac = particle.X - i_j.X * cellsize;
        float j_frac = particle.Y - i_j.Y * cellsize;

        //2. ���� cell�� �������� ������, ��, �������� cell�� �ҷ�����
        float i_plus1 = grid.CLAMP(i_j.X + 1, 0, gridsize - 1);
        float j_plus1 = grid.CLAMP(i_j.Y + 1, 0, gridsize - 1);
        T value_i_j = grid.cell_values[grid.get_VectorIndex_from_cell(i_j)];
        T value_iplus1_j = grid.cell_values[grid.get_VectorIndex_from_cell(Vector2D(i_plus1, i_j.Y))];
        T value_i_jplus1 = grid.cell_values[grid.get_VectorIndex_from_cell(Vector2D(i_j.X, j_plus1))];
        T value_iplus1_jplus1 = grid.cell_values[grid.get_VectorIndex_from_cell(Vector2D(i_plus1, j_plus1))];

        //������ ���� interpolate
        //i,j �� i+1,j ����
        T value_y0 = value_i_j * (cellsize - i_frac) / cellsize + value_iplus1_j * i_frac / cellsize;
        //i,j+1�� i+1,j+1����
        T value_y1 = value_i_jplus1 * (cellsize - i_frac) / cellsize + value_iplus1_jplus1 * i_frac / cellsize;

        //�� ���� y�� ����
        T result = value_y0 * (cellsize - j_frac) / cellsize + value_y1 * j_frac / cellsize;
        return result;
    }

    template <class T>
    void extrapolation_value_to_grid_from_particle(MAC_Grid<T>& grid, Vector2D particle, T value) {
        //1. cell �ȿ����� particle�� ��ġã��
        //1-1. particle�� cell��ǥ ã��
        Vector2D i_j = grid.get_cell_i_j_from_world(particle);
        //1-2. particle�� cell�� ���ʸ�, �Ʒ������ �Ÿ� ���ϱ�
        float i_frac = particle.X - i_j.X * cellsize;
        float j_frac = particle.Y - i_j.Y * cellsize;


        //2. ���� cell�� �������� ������, ��, �������� cell�� �ҷ�����
        float i_plus1 = grid.CLAMP(i_j.X + 1, 0, gridsize - 1);
        float j_plus1 = grid.CLAMP(i_j.Y + 1, 0, gridsize - 1);

        //y�������� extrapolation - interpolation�� ����
        T value_y0 = value * (cellsize - j_frac) / cellsize;
        T value_y1 = value * j_frac / cellsize;

        //x�������� extrapolation
        T value_i_j = value_y0 * (cellsize - i_frac) / cellsize;
        T value_iplus1_j = value_y0 * i_frac / cellsize;
        T value_i_jplus1 = value_y1 * (cellsize - i_frac) / cellsize;
        T value_iplus1_jplus1 = value_y1 * i_frac / cellsize;

        //add values to grid
        grid.cell_values[grid.get_VectorIndex_from_cell(i_j)] = grid.cell_values[grid.get_VectorIndex_from_cell(i_j)] + value_i_j;
        grid.cell_values[grid.get_VectorIndex_from_cell(Vector2D(i_plus1, i_j.Y))] = grid.cell_values[grid.get_VectorIndex_from_cell(Vector2D(i_plus1, i_j.Y))] + value_iplus1_j;
        grid.cell_values[grid.get_VectorIndex_from_cell(Vector2D(i_j.X, j_plus1))] = grid.cell_values[grid.get_VectorIndex_from_cell(Vector2D(i_j.X, j_plus1))] + value_i_jplus1;
        grid.cell_values[grid.get_VectorIndex_from_cell(Vector2D(i_plus1, j_plus1))] = grid.cell_values[grid.get_VectorIndex_from_cell(Vector2D(i_plus1, j_plus1))] + value_iplus1_jplus1;

    }

    void transfer_Velocity_to_particle_from_grid() {

        //for (int p = 0; p < particles.size(); p++) {
        //    //1. particle�� ���� cell ã��
        //    Vector2D i_j = velocity_grid.get_cell_i_j_from_world(particles[p].Location);

        //    //2. cell ������ particle���� ����
        //    particles[p].Velocity = interpolate_value_to_particle_from_grid(velocity_grid, particles[p].Location);

        //}
        //================================
        for (int p = 0; p < particles.size(); p++) {
            //1. particle�� ���� cell ã��
            Vector2D i_j = velocity_grid.get_cell_i_j_from_world(particles[p].Location);

            //2. particle�� weight�ٽ� ���ϱ�
            float weight = 1.0 / particles[p].get_distance_from_point(cell_center_point.cell_values[cell_center_point.get_VectorIndex_from_cell(i_j)]);

            //3. particle�� velocity�� weight���� / weight�� ���ؼ� ����
            particles[p].Velocity = velocity_grid.cell_values[velocity_grid.get_VectorIndex_from_cell(i_j)] * weight_velocity_grid.cell_values[velocity_grid.get_VectorIndex_from_cell(i_j)] / weight;
        }
    }


    //====================Debugging=========================

    void print_particles(Particle2D particle) {
        cout << "===========================simulation start=====================================";
        cout << endl;
        cout << "this is 2 particle" << endl;
        cout << "Location :" << particle.Location.X << "," << particle.Location.Y << endl;
        cout << "Velocity :" << particle.Velocity.X << "," << particle.Velocity.Y << endl;
        cout << "Acceleration :" << particle.Acceleration.X << "," << particle.Acceleration.Y << endl;
        cout << endl;
        cout << "===========================simulation end=====================================" << endl << endl;
    }

    template <class T>
    void print_grid_Vector(MAC_Grid<T> grid) {
        cout << endl;
        for (int i = 0; i < gridsize * gridsize; i++) {
            cout << grid.cell_values[i].X << "," << grid.cell_values[i].Y;
            cout << std::left << "===";
            if (i % gridsize == gridsize - 1) {
                cout << endl;
            }
        }
    }

    template <class T>
    void print_grid_Scalar(MAC_Grid<T> grid) {
        cout << endl;
        for (int i = 0; i < gridsize * gridsize; i++) {
            cout.width(15);
            cout << std::left << grid.cell_values[i] << " ";
            if (i % gridsize == gridsize - 1) {
                cout << endl;
            }
        }
    }



};