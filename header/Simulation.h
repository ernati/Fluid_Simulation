#pragma once

#include "myVector2D.h"
#include "particle.h"
#include "grid.h"
#include <vector>

//난수 생성
#include<cstdlib> //rand(), srand()
#include<ctime>

using namespace std;

//등가속도 운동 시뮬레이션 - v_n+1_ = v_n_ + a*t
class Constant_Acceleration_Simulator {

public:

    //particle들을 담을 vector
    vector<Particle2D> particles;
    float timestep;

    MAC_Grid<Vector2D> velocity_grid;


    //아무 입력값이 없다면 기본 particle 수는 100개, grid_N = 10
    Constant_Acceleration_Simulator() {

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

    Constant_Acceleration_Simulator(int particle_number, int grid_N) {

        //입자들 초기화
        srand((unsigned int)time(NULL));
        for (int i = 0; i < particle_number; i++) {
            //0~99 난수 생성    
            int randomLocation_X = rand() % 100;
            //0~49 난수 생성
            int randomLocation_Y = rand() % 50;

            //위치 X : 0.0~ 0.99, 위치 Y : 0.5 ~ 0.99
            Particle2D tmp = Particle2D((float)randomLocation_X / 100.0, (float)randomLocation_Y / 100.0 + 0.5, 0, 0, 0, -0.098);
            particles.push_back(tmp);
        }

        //velocity grid 그리기
        velocity_grid = MAC_Grid<Vector2D>(grid_N);

        //timestep의 default = 0.06
        timestep = 0.06;
    }

    void particle_simulation() {
        //속도 update
        Update_particles_Velocity();

        //위치 update
        Update_particles_Location();

    }


    //입자들의 속도를 모두 update
    void Update_particles_Velocity() {
        for (int i = 0; i < particles.size(); i++) {
            this->particles[i].Update_particle_Velocity(timestep);
        }
    }

    //입자들의 위치를 모두 update
    void Update_particles_Location() {
        for (int i = 0; i < particles.size(); i++) {
            this->particles[i].Update_particle_Location(timestep);
        }
    }



};

class Fluid_Simulator {

public:

    //particle들을 담을 vector
    vector<Particle2D> particles;
    float timestep;
    int gridsize;

    //grid 한변의 길이
    float cellsize;

    //cell들의 속도를 담을 grid
    MAC_Grid<Vector2D> velocity_grid;

    //cell들의 가속도를 담을 grid - 외력
    MAC_Grid<Vector2D> bodyforce;

    //cell들의 중심좌표를 담을 grid
    MAC_Grid<Vector2D> cell_center_point;

    //cell별로 속도를 담을때 필요한 weight들을 담을 grid
    MAC_Grid<float> weight_velocity_grid;

    //cell별로 particle의 수를 담을 grid
    MAC_Grid<int> the_number_of_particles_in_cell;


    Fluid_Simulator() {
        //timestep의 default = 0.06
        timestep = 0.000006;

        gridsize = 10;

        cellsize = 1.0 / (float)gridsize;

        //velocity grid 초기화
        velocity_grid = MAC_Grid<Vector2D>(gridsize);
        for (int n = 0; n < (gridsize) * (gridsize); n++) {
            velocity_grid.cell_values.push_back(Vector2D());
        }
    }

    Fluid_Simulator(int particle_number, int grid_N) {

        //timestep의 default = 0.06
        timestep = 0.06;

        gridsize = grid_N;

        cellsize = 1.0 / (float)gridsize;

        //입자들 초기화
        srand((unsigned int)time(NULL));
        for (int i = 0; i < particle_number; i++) {
            //0~99 난수 생성    
            int randomLocation_X = rand() % 100;
            //0~49 난수 생성
            int randomLocation_Y = rand() % 50;

            //위치 X : 0.0~ 0.99, 위치 Y : 0.5 ~ 0.99
            Particle2D tmp = Particle2D((float)randomLocation_X / 100.0, (float)randomLocation_Y / 100.0 + 0.5, 0, 0, 0, -0.098);
            particles.push_back(tmp);
        }

        //velocity grid 초기화
        velocity_grid = MAC_Grid<Vector2D>(gridsize);
        for (int n = 0; n < (gridsize) * (gridsize); n++) {
            velocity_grid.cell_values.push_back(Vector2D());
        }

        //body force 초기화
        bodyforce = MAC_Grid<Vector2D>(gridsize);
        for (int n = 0; n < (gridsize) * (gridsize); n++) {
            bodyforce.cell_values.push_back(Vector2D());
        }

        //cell_center_grid 초기화
        cell_center_point = MAC_Grid<Vector2D>(gridsize);
        //cell들에다가 cell_point정보 삽입
        //1. cell의 갯수는 gridsize의 제곱
        for (int v = 0; v < (gridsize) * (gridsize); v++) {
            //2. (0,0) 은 ( 1/gridsize , 1/ gridsize )
            if (v == 0) {
                cell_center_point.cell_values.push_back(Vector2D(cellsize / 2.0, cellsize / 2.0));
            }
            //3. (i,j)는 ( 1/gridsize + i * gridsize , 1/gridsize + j* gridsize )
            else {
                //3-1. v ( vector Index ) to (i,j)
                int i = cell_center_point.get_cell_i_from_VectorIndex(v);
                int j = cell_center_point.get_cell_j_from_VectorIndex(v);

                //3-2. 식 적용
                cell_center_point.cell_values.push_back(Vector2D(cellsize / 2.0 + i * cellsize, cellsize / 2.0 + j * cellsize));
            }
        }

        //weight grid 초기화
        weight_velocity_grid = MAC_Grid<float>(gridsize);
        for (int n = 0; n < (gridsize) * (gridsize); n++) {
            weight_velocity_grid.cell_values.push_back(0.0);
        }

        //cell별로 particle의 수를 담을 grid
        the_number_of_particles_in_cell = MAC_Grid<int>(gridsize);
        for (int n = 0; n < (gridsize) * (gridsize); n++) {
            the_number_of_particles_in_cell.cell_values.push_back( 0 );
        }



    }

    void particle_simulation() {

        //particle to grid
        transfer_velocity_to_grid_from_particle();

        //grid에 bodyforce 추가
        add_body_force();

        //bodyforce를 velocity에 적용
        apply_bodyforce_to_Velocity();

        //grid to particle - Velocity
        transfer_Velocity_to_particle_from_grid();

        //grid to particle - bodyforce(Acceleration) - 
        transfer_bodyforce_to_particle_from_grid();

        //Update particle Location
        Update_particles_Location();

    }

    //bodyforce에 적용하고 싶은 외력 추가 - 중력
    void add_body_force() {
        for (int i = 0; i < (gridsize * gridsize); i++) {
            bodyforce.cell_values[i] = Vector2D(0.0, -0.0000098);
        }
    }

    //bodyforce를 velocity grid에 적용 - 등가속도
    void apply_bodyforce_to_Velocity() {
        for (int i = 0; i < (gridsize * gridsize); i++) {
            velocity_grid.cell_values[i] = velocity_grid.cell_values[i] + bodyforce.cell_values[i] * timestep;
        }
    }

    //일괄적으로 같은 값을 전달받아도 되지만, 일단 포함된 cell의 정보를 받을 수 있게 해놓음.
    void transfer_bodyforce_to_particle_from_grid() {
        for (int p = 0; p < particles.size(); p++) {
            //1. particle이 어디 cell인지 파악
            Vector2D i_j = bodyforce.get_cell_i_j_from_world(particles[p].Location);

            //2. bodyforce cell값을 Accceleration에 전달
            particles[p].Acceleration = bodyforce.cell_values[bodyforce.get_VectorIndex_from_cell(i_j)];
        }
    }

    //입자들의 속도를 모두 update
    void Update_particles_Velocity() {
        for (int i = 0; i < particles.size(); i++) {
            this->particles[i].Update_particle_Velocity(timestep);
        }
    }

    //입자들의 위치를 모두 update
    void Update_particles_Location() {
        for (int i = 0; i < particles.size(); i++) {
            this->particles[i].Update_particle_Location(timestep);
        }
    }

    //particle의 value를 모든 grid로 전달하는 함수
    void transfer_velocity_to_grid_from_particle() {
        ////0. the number of particles in cell grid 초기화
        //for (int i = 0; i < gridsize * gridsize; i++) {
        //    the_number_of_particles_in_cell.cell_values[i] = 0;
        //}

        //for (int p = 0; p < particles.size(); p++) {
        //    //1. particle cell좌표 찾기
        //    Vector2D i_j = velocity_grid.get_cell_i_j_from_world(particles[p].Location);

        //    //2. cell에 particle의 수 담기
        //    the_number_of_particles_in_cell.cell_values[the_number_of_particles_in_cell.get_VectorIndex_from_cell(i_j)]++;

        //    //3. velocity cell값을 주변 grid로 전달
        //    extrapolation_value_to_grid_from_particle(velocity_grid, particles[p].Location, velocity_grid.cell_values[velocity_grid.get_VectorIndex_from_cell(i_j)]);

        //}

        ////4. particle의 수만큼 속도에서 나눈다.
        //for (int i = 0; i < gridsize * gridsize; i++) {
        //    if (the_number_of_particles_in_cell.cell_values[i] == 0) continue;
        //    velocity_grid.cell_values[i] = velocity_grid.cell_values[i] / (float)the_number_of_particles_in_cell.cell_values[i];
        //}

        //========================================

        for (int p = 0; p < particles.size(); p++) {
            //1. particle cell 좌표 찾기
            Vector2D i_j = velocity_grid.get_cell_i_j_from_world(particles[p].Location);

            //2. particle과 cell의 중점 좌표 거리 구하기
            float weight = 1.0 / particles[p].get_distance_from_point(cell_center_point.cell_values[ cell_center_point.get_VectorIndex_from_cell( i_j ) ] );
            weight_velocity_grid.cell_values[weight_velocity_grid.cell_values[cell_center_point.get_VectorIndex_from_cell(i_j)]] = weight_velocity_grid.cell_values[weight_velocity_grid.cell_values[cell_center_point.get_VectorIndex_from_cell(i_j)]] + weight;

            //3. weight를 particle의 속도와 곱해서 velocity_grid에 더하기
            velocity_grid.cell_values[velocity_grid.get_VectorIndex_from_cell(i_j)] = velocity_grid.cell_values[velocity_grid.get_VectorIndex_from_cell(i_j)] + velocity_grid.cell_values[velocity_grid.get_VectorIndex_from_cell(i_j)] * weight;
        }

        //4. weight총합을 모든 grid와 나눠주기
        for (int i = 0; i < gridsize * gridsize; i++) {
            if (weight_velocity_grid.cell_values[i] == 0) { continue; }
            velocity_grid.cell_values[i] = velocity_grid.cell_values[i] / weight_velocity_grid.cell_values[i];
        }

    }

    //particle의 좌표에 따라 value값을 주변 grid로부터 받아오는 함수
    template <class T>
    T interpolate_value_to_particle_from_grid(MAC_Grid<T> grid, Vector2D particle) {
        //1. cell 안에서의 particle의 위치찾기
        //1-1. particle의 cell좌표 찾기
        Vector2D i_j = grid.get_cell_i_j_from_world(particle);
        //1-2. particle과 cell의 왼쪽면, 아랫면과의 거리 구하기
        float i_frac = particle.X - i_j.X * cellsize;
        float j_frac = particle.Y - i_j.Y * cellsize;

        //2. 현재 cell을 기준으로 오른쪽, 위, 오른쪽위 cell들 불러오기
        float i_plus1 = grid.CLAMP(i_j.X + 1, 0, gridsize - 1);
        float j_plus1 = grid.CLAMP(i_j.Y + 1, 0, gridsize - 1);
        T value_i_j = grid.cell_values[grid.get_VectorIndex_from_cell(i_j)];
        T value_iplus1_j = grid.cell_values[grid.get_VectorIndex_from_cell(Vector2D(i_plus1, i_j.Y))];
        T value_i_jplus1 = grid.cell_values[grid.get_VectorIndex_from_cell(Vector2D(i_j.X, j_plus1))];
        T value_iplus1_jplus1 = grid.cell_values[grid.get_VectorIndex_from_cell(Vector2D(i_plus1, j_plus1))];

        //비율에 따른 interpolate
        //i,j 와 i+1,j 보간
        T value_y0 = value_i_j * (cellsize - i_frac) / cellsize + value_iplus1_j * i_frac / cellsize;
        //i,j+1과 i+1,j+1보간
        T value_y1 = value_i_jplus1 * (cellsize - i_frac) / cellsize + value_iplus1_jplus1 * i_frac / cellsize;

        //두 값을 y로 보간
        T result = value_y0 * (cellsize - j_frac) / cellsize + value_y1 * j_frac / cellsize;
        return result;
    }

    template <class T>
    void extrapolation_value_to_grid_from_particle(MAC_Grid<T>& grid, Vector2D particle, T value) {
        //1. cell 안에서의 particle의 위치찾기
        //1-1. particle의 cell좌표 찾기
        Vector2D i_j = grid.get_cell_i_j_from_world(particle);
        //1-2. particle과 cell의 왼쪽면, 아랫면과의 거리 구하기
        float i_frac = particle.X - i_j.X * cellsize;
        float j_frac = particle.Y - i_j.Y * cellsize;


        //2. 현재 cell을 기준으로 오른쪽, 위, 오른쪽위 cell들 불러오기
        float i_plus1 = grid.CLAMP(i_j.X + 1, 0, gridsize - 1);
        float j_plus1 = grid.CLAMP(i_j.Y + 1, 0, gridsize - 1);

        //y방향으로 extrapolation - interpolation의 역순
        T value_y0 = value * (cellsize - j_frac) / cellsize;
        T value_y1 = value * j_frac / cellsize;

        //x방향으로 extrapolation
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
        //    //1. particle이 속한 cell 찾기
        //    Vector2D i_j = velocity_grid.get_cell_i_j_from_world(particles[p].Location);

        //    //2. cell 정보를 particle에게 전달
        //    particles[p].Velocity = interpolate_value_to_particle_from_grid(velocity_grid, particles[p].Location);

        //}
        //================================
        for (int p = 0; p < particles.size(); p++) {
            //1. particle이 속한 cell 찾기
            Vector2D i_j = velocity_grid.get_cell_i_j_from_world(particles[p].Location);

            //2. particle의 weight다시 구하기
            float weight = 1.0 / particles[p].get_distance_from_point(cell_center_point.cell_values[cell_center_point.get_VectorIndex_from_cell(i_j)]);

            //3. particle의 velocity를 weight총합 / weight랑 곱해서 대입
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