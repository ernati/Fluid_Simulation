#pragma once

#include "../header_background/myVector3D.h"
#include "../header_background/particle3D.h"
#include "../header_background/grid3D.h"
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

enum CellType {
    FLUID,
    AIR,
    SOLID
};

//sort
bool compare(Particle3D& a, Particle3D& b) {
    return (a.Location.X < b.Location.X);
}

//sort
bool compare2(Vector3D& a, Vector3D& b) {
    return (a.X < b.X);
}

//sort
bool compare3(tuple<Vector3D, int>& a, tuple<Vector3D, int>& b) {
    return (get<0>(a).X < get<0>(b).X);
}


//등가속도 운동 시뮬레이션 - v_n+1_ = v_n_ + a*t


class Fluid_Simulator_Grid {

public:

    //particle들을 담을 vector
    vector<Particle3D> particles;

    int particle_number;
    double timestep;
    int gridsize;

    double density;

    double delta_x;
    double delta_y;
    double delta_z;

    //cell 총개수
    int cell_number;

    //cell들의 속도를 담을 grid
    MAC_Grid<Vector3D>* previous_velocity_grid;
    MAC_Grid<Vector3D>* next_velocity_grid;

    //cell들의 가속도를 담을 grid - 외력
    MAC_Grid<Vector3D>* bodyforce;

    //cell 안의 포함된 particle 수를 담을 grid
    MAC_Grid<double>* cell_particle_number;

    //divergence 계산을 위한 velocity 차 grid
    MAC_Grid<double>* velocity_difference_X_grid;
    MAC_Grid<double>* velocity_difference_Y_grid;
    MAC_Grid<double>* velocity_difference_Z_grid;

    //cell들의 중심좌표를 담을 grid
    MAC_Grid<Vector3D>* cell_center_point;

    //유체가 들어있는 cell들의 위치를 담을 grid
    MAC_Grid<CellType>* cell_type_grid;

    //유체가 들어있는 cell들의 중심좌표를 담을 vector
    vector<Vector3D>* fluid_cell_center_point;
    //fluid로부터 속도를 extrapolation 받은 AIR cell들의 중심좌표를 담을 vector
    vector<Vector3D>* air_cell_center_point;

    //grid 좌표 함수들을 위한 tool
    MAC_Grid<bool>* tool;

    //사용할 행렬 변수들
    Eigen::SparseMatrix<double>* A;


    Fluid_Simulator_Grid() {
    }

    Fluid_Simulator_Grid(int particle_number, int grid_N) {
        this->particle_number = particle_number;
        init(this->particle_number, grid_N);
    }

    //===========================================init and clear===========================================
    void init(int particle_number, int grid_N) {
        //timestep의 default = 0.06

        gridsize = grid_N;

        cell_number = gridsize * gridsize * gridsize;

        //입자들 초기화
        srand((unsigned int)time(NULL));
        for (int i = 0; i < particle_number; i++) {
            //0~99 난수 생성    
            int randomLocation_X = rand() % 200;
            //0~99 난수 생성
            int randomLocation_Y = rand() % 200;

            //위치 X : 0.1~0.3, 위치 Y : 0.6~0.8
            Particle3D tmp = Particle3D(((double)randomLocation_X + 100) / 1000.0, (((double)randomLocation_Y) + 600) / 1000.0, (((double)randomLocation_Y) + 600) / 1000.0,  (double)randomLocation_X / 500.0, (double)randomLocation_Y / 500.0, (double)randomLocation_Y / 500.0, 0,0, -0.98);
            particles.push_back(tmp);

        }

        //timestep의 default = 0.06
        timestep = 0.06;
        density = 1.0;
        delta_x = 1.0 / (double)grid_N;
        delta_y = 1.0 / (double)grid_N;
        delta_z = 1.0 / (double)grid_N;

        tool = new MAC_Grid<bool>(gridsize);

        //previous_velocity grid 초기화
        previous_velocity_grid = new MAC_Grid<Vector3D>(gridsize);
        for (int n = 0; n < cell_number; n++) {
            previous_velocity_grid->cell_values.push_back(Vector3D());
        }

        //previous_velocity grid 초기화
        next_velocity_grid = new MAC_Grid<Vector3D>(gridsize);
        for (int n = 0; n < cell_number; n++) {
            next_velocity_grid->cell_values.push_back(Vector3D());
        }

        //body force 초기화
        bodyforce = new MAC_Grid<Vector3D>(gridsize);
        for (int n = 0; n < cell_number; n++) {
            bodyforce->cell_values.push_back(Vector3D());
        }

        //cell_particle_number 초기화
        cell_particle_number = new MAC_Grid<double>(gridsize);
        for (int n = 0; n < cell_number; n++) {
            cell_particle_number->cell_values.push_back(0.0);
        }

        //velocity_difference_grid 초기화
        velocity_difference_X_grid = new MAC_Grid<double>(gridsize);
        velocity_difference_Y_grid = new MAC_Grid<double>(gridsize);
        velocity_difference_Z_grid = new MAC_Grid<double>(gridsize);

        for (int n = 0; n < cell_number; n++) {
            velocity_difference_X_grid->cell_values.push_back(0.0);
            velocity_difference_Y_grid->cell_values.push_back(0.0);
            velocity_difference_Z_grid->cell_values.push_back(0.0);
        }

        // 행렬 초기화
        A = new Eigen::SparseMatrix<double>(cell_number, cell_number);
        //A = Eigen::SparseMatrix<double>(cell_number, cell_number);

        //cell_center_grid 초기화
        cell_center_point = new MAC_Grid<Vector3D>(gridsize);
        //cell들에다가 cell_point정보 삽입
        //1. cell의 갯수는 gridsize의 세제곱
        for (int v = 0; v < cell_number; v++) {
            //2. (0,0) 은 ( 1/gridsize , 1/ gridsize )
            if (v == 0) {
                cell_center_point->cell_values.push_back(Vector3D(delta_x / 2.0, delta_y / 2.0, delta_z / 2.0));
            }
            //3. (i,j)는 ( 1/gridsize + i * gridsize , 1/gridsize + j* gridsize )
            else {
                //3-1. v ( vector Index ) to (i,j)
                int i = tool->get_cell_i_from_VectorIndex(v);
                int j = tool->get_cell_j_from_VectorIndex(v);
                int k = tool->get_cell_k_from_VectorIndex(v);

                //3-2. 식 적용
                cell_center_point->cell_values.push_back(Vector3D(delta_x / 2.0 + i * delta_x, delta_y / 2.0 + j * delta_y, delta_y / 2.0 + j * delta_z ));
            }
        }

        //cell_type_grid 초기화
        cell_type_grid = new MAC_Grid<CellType>(gridsize);
        for (int n = 0; n < cell_number; n++) {
            cell_type_grid->cell_values.push_back(CellType::AIR);
        }

        //fluid_cell_center_point 초기화
        fluid_cell_center_point = new vector<Vector3D>;

        //air_cell_center_point 초기화
        air_cell_center_point = new vector<Vector3D>;
    }

    //모든 정보를 초기화
    void clear_and_ReInit() {
        clear();

        init(particle_number, gridsize);
    }

    void clear() {
        //vector들을 clear
        previous_velocity_grid->cell_values.clear();
        next_velocity_grid->cell_values.clear();
        bodyforce->cell_values.clear();
        cell_particle_number->cell_values.clear();
        velocity_difference_X_grid->cell_values.clear();
        velocity_difference_Y_grid->cell_values.clear();
        cell_center_point->cell_values.clear();
        cell_type_grid->cell_values.clear();
        fluid_cell_center_point->clear();
        air_cell_center_point->clear();
        particles.clear();

        A->setZero();

    }

    void delete_vectors() {
        delete previous_velocity_grid;
        delete next_velocity_grid;
        delete bodyforce;
        delete cell_particle_number;
        delete velocity_difference_X_grid;
        delete velocity_difference_Y_grid;
        delete cell_center_point;
        delete cell_type_grid;
        delete fluid_cell_center_point;
        delete air_cell_center_point;
        delete A;
    }

    //================================================================================================

    //========================================simulation=============================================

    void particle_simulation() {

        //0. collision detection
        collision_detection();

        cout << "==========================================================" << endl;

        cout << " boundarycondition start " << endl;

        //1. boundarycondition
        boundarycondition_particle();

        cout << " advection start " << endl;

        //2. advection
        advection();

        cout << "==============================================================================" << endl;

        cout << " transfer_velocity_to_grid_from_particle start " << endl;

        //3. transfer_velocity_to_grid_from_particle
        transfer_velocity_to_grid_from_particle();

        //4. cell 성질 분류
        classify_cell_type();

        cout << " bodyforce start " << endl;

        //4. bodyforce cell에 추가
        add_body_force();

        cout << " Adjust_velocity_from_bodyforce start " << endl;

        //5. bodyforce 적용
        Adjust_velocity_from_bodyforce();

        //7. 유체주변 셀에 보간
        extrapolate_velocity_to_air_cell();


        cout << " pressureSolve start " << endl;

        //8. 압력 계산
        pressure_solve();

        cout << " transfer_velocity_to_particle_from_grid start " << endl;

        //9. boundarycondition for grid
        boundarycondition_grid();

        //10. transfer_velocity_to_grid_from_particle
        transfer_Velocity_to_particle_from_grid();

        //11. swap buffer
        cout << " swapbuffer start " << endl;

        swap_buffer();


        //12. 유체가 있는 cell들 색칠
        rendering_fluid();

    }

    //================================================================================================

    //========================================collision detection=============================================

    //collision detection
    void collision_detection() {
        vector< tuple<Vector3D, int> >* temp_particles = new vector< tuple<Vector3D, int> >;
        vector< tuple<int, int>  >* temp_particles2 = new vector< tuple<int, int>  >;

        vector<bool>* collision_check = new vector<bool>;

        //0. 초기화
        for (int i = 0; i < particles.size(); i++) {
            collision_check->push_back(false);
        }


        //1. 예상 위치 계산
        for (int p = 0; p < particles.size(); p++) {
            Vector3D expecting_Location = this->particles[p].Location + this->particles[p].Velocity * timestep;
            temp_particles->push_back(make_tuple(expecting_Location, p));
        }

        //2. particle들을 sort
        sort(temp_particles->begin(), temp_particles->end(), compare3);

        //3. 충돌 가능성 있는 particle들을 구분 - sweep and prune
        for (int i = 0; i < temp_particles->size(); i++) {
            //3-1. 앞에서 하나씩 훑는데, 자기 다음 particle과의 거리가 radius보다 작으면 충돌 가능성이 있음.
            //이미 분류된 particle이면 pass
            if ((*collision_check)[std::get<1>((*temp_particles)[i])]) { continue; }
            //i가 마지막 particle이면 break
            else if (i == temp_particles->size() - 1) { break; }

            //현재 particle과 다음 particle의 거리가 0이상 2*radius 이하이면 충돌 가능성이 있음.
            else {
                double distance = sqrt(pow(get<0>((*temp_particles)[i]).X - get<0>((*temp_particles)[i + 1]).X, 2) + pow(get<0>((*temp_particles)[i]).Y - get<0>((*temp_particles)[i + 1]).Y, 2) + pow(get<0>((*temp_particles)[i]).Z - get<0>((*temp_particles)[i + 1]).Z, 2) );
                if (distance <= 2 * 0.05) {
                    //충돌 가능성이 있는 particle들을 저장
                    temp_particles2->push_back(make_tuple(get<1>((*temp_particles)[i]), get<1>((*temp_particles)[i + 1])));
                    //충돌 가능성이 있는 particle들은 true로 바꿔줌
                    (*collision_check)[std::get<1>((*temp_particles)[i])] = true;
                    (*collision_check)[std::get<1>((*temp_particles)[i + 1])] = true;
                }
            }
        }

        // 4. 충돌 가능성 있는 particle들끼리 모멘텀 보존을 통한 충돌 후 속도 update를 적용
        for (int i = 0; i < temp_particles2->size(); i++) {
            tuple<int, int> tmp = (*temp_particles2)[i];
            Vector3D v1 = this->particles[get<0>(tmp)].Velocity;
            Vector3D v2 = this->particles[get<1>(tmp)].Velocity;

            //두 입자의 질량이 같으므로 뺄셈 생략
            Vector3D new_v1 = (v2 * 2 * this->particles[get<1>(tmp)].mass) / (this->particles[get<0>(tmp)].mass + this->particles[get<1>(tmp)].mass);
            Vector3D new_v2 = (v1 * 2 * this->particles[get<0>(tmp)].mass) / (this->particles[get<0>(tmp)].mass + this->particles[get<1>(tmp)].mass);

            //속도 update
            this->particles[get<0>(tmp)].Velocity = new_v1;
            this->particles[get<1>(tmp)].Velocity = new_v2;
        }


        //delete
        delete temp_particles;
        delete temp_particles2;
        delete collision_check;
    }

    //==============================boundary 관련 함수==============================

    //압력 계산 후의 속도가 boundarycondition을 벗어나는 경우 속도를 적당한 조건으로 변경함.
    void boundarycondition_grid() {
        for (int i = 0; i < cell_number; i++) {
            Vector3D expecting_Location = this->cell_center_point->cell_values[i] + this->next_velocity_grid->cell_values[i] * timestep;

            int check = check_location_for_boundary(expecting_Location);

            boundary_work(check, this->next_velocity_grid->cell_values[i]);
        }
    }

    //속도에 따른 위치 예상 결과값이 boundarycondition을 벗어나는 경우 속도를 적당한 조건으로 변경함.
    void boundarycondition_particle() {
        for (int i = 0; i < particles.size(); i++) {
            Vector3D expecting_Location = this->particles[i].Location + this->particles[i].Velocity * timestep;

            int check = check_location_for_boundary(expecting_Location);

            boundary_work(check, particles[i].Velocity);
        }
    }

    int check_location_for_boundary(Vector3D location) {
        if (location.X < delta_x) { return 1; }
        if (location.X > delta_x * (gridsize - 1)) { return 2; }
        if (location.Y < delta_y) { return 3; }
        if (location.Y > delta_y * (gridsize - 1)) { return 4; }
        if (location.Z < delta_z) { return 5; }
        if (location.Z > delta_z * (gridsize - 1)) { return 6; }

        return 0;
    }

    void boundary_work(int check, Vector3D& Velocity) {
        if (check == 1) {
            Velocity.X *= -0.95;
            Velocity.Y *= 1.0;
            Velocity.Z *= 1.0;
        }
        else if (check == 2) {
            Velocity.X *= -0.95;
            Velocity.Y *= 1.0;
            Velocity.Z *= 1.0;
        }
        else if (check == 3) {
            Velocity.X *= 1.0;
            Velocity.Y *= -0.65;
            Velocity.Z *= 1.0;
        }
        else if (check == 4) {
            Velocity.X *= 1.0;
            Velocity.Y *= -0.95;
            Velocity.Y *= 1.0;
        }
        else if (check == 5) {
            Velocity.X *= 1.0;
            Velocity.Y *= 1.0;
            Velocity.Z *= -0.95;
        }
        else {
            Velocity.X *= 1.0;
            Velocity.Y *= 1.0;
            Velocity.Z *= -0.95;
        }


    }



    //==============================================================================

    //==============================advection=================================


    //입자들의 위치를 모두 update
    void advection() {
        for (int i = 0; i < particles.size(); i++) {
            this->particles[i].Location = this->particles[i].Location + this->particles[i].Velocity * timestep;

            //boundary condition : 입자 위치 강제 이동 추가
            if (check_location_for_boundary(this->particles[i].Location)) {
                srand((unsigned int)time(NULL));

                // x 는 0.15~0.85 사이의 랜덤값 , y는 0.15 ~ 0.35 사이의 랜덤값
                double random_x = (rand() % 70 + 15) / 100.0;
                double random_y = (rand() % 20 + 15) / 100.0;
                double random_z = (rand() % 20 + 15) / 100.0;

                this->particles[i].Location = Vector3D(random_x, random_y, random_z);
            }
        }
    }

    //==============================================================================


    //==============================transferVelocity=================================

    int check_particle_on_the_plane(Vector3D Location, double threshold) {
        //0. edge들과의 거리 체크
        Vector3D i_j_k = tool->get_cell_i_j_k_from_world(Location);
        double left_distance_x = abs(Location.X - delta_x * i_j_k.X);
        double right_distance_x = abs(Location.X - delta_x * (i_j_k.X + 1));
        double down_distance_y = abs(Location.Y - delta_y * i_j_k.Y);
        double up_distance_y = abs(Location.Y - delta_y * (i_j_k.Y + 1));
        double down_distance_z = abs(Location.Z - delta_z * i_j_k.Z);
        double up_distance_z = abs(Location.Z - delta_z * (i_j_k.Z + 1));

        //1. j_plus 면과 가까움
        if (up_distance_y < threshold && left_distance_x > threshold && right_distance_x > threshold && down_distance_z > threshold && up_distance_z > threshold) { return 1; }
        //2. j면과 가까움
        if (down_distance_y < threshold && left_distance_x > threshold && right_distance_x > threshold && down_distance_z > threshold && up_distance_z > threshold) { return 2; }
        //3. i_pus면과 가까움
        if (right_distance_x < threshold && down_distance_y > threshold && up_distance_y > threshold && down_distance_z > threshold && up_distance_z > threshold) { return 3; }
        //4. i면과 가까움
        if (left_distance_x < threshold && down_distance_y > threshold && up_distance_y > threshold && down_distance_z > threshold && up_distance_z > threshold) { return 4; }
        //5. k_plus 면과 가까움
        if (up_distance_z < threshold && left_distance_x > threshold && right_distance_x > threshold && down_distance_y > threshold && up_distance_y > threshold) { return 1; }
        //6. k면과 가까움
        if (down_distance_z < threshold && left_distance_x > threshold && right_distance_x > threshold && down_distance_y > threshold && up_distance_y > threshold) { return 2; }

        return 0;
    }



    //particle의 value를 모든 grid로 전달하는 함수
    void transfer_velocity_to_grid_from_particle() {

        //0. cell_particle_number,previous_velocity_grid 초기화
        for (int i = 0; i < cell_number; i++) {
            cell_particle_number->cell_values[i] = 0.0;
            previous_velocity_grid->cell_values[i] = Vector3D();
        }

        for (int p = 0; p < particles.size(); p++) {
            //1. particle cell좌표 찾기
            Vector3D i_j_k = tool->get_cell_i_j_k_from_world(particles[p].Location);

            //cell 내부 particle이 15개 초과일 경우 밑의 과정 생략
            if (cell_particle_number->cell_values[tool->get_VectorIndex_from_cell(i_j_k)] > 15) { continue; }

            //2. cell에 포함된 particle 수 세기
            cell_particle_number->cell_values[tool->get_VectorIndex_from_cell(i_j_k)] = cell_particle_number->cell_values[tool->get_VectorIndex_from_cell(i_j_k)] + 1.0;

            //3. cell에 포함된 particle의 속도 더하기 - edge처리 포함
            double check = check_particle_on_the_plane(particles[p].Location, 0.01);

            //3-0. edge에 없는 경우
            if (check == 0) {
                previous_velocity_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_k)] = previous_velocity_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_k)] + particles[p].Velocity;
            }

            //edge에 있는 경우
            else {
                //3-1. j_plus 면과 가까움
                if (check == 1) {
                    Vector3D i_j_plus_1_k = Vector3D(i_j_k.X, i_j_k.Y + 1, i_j_k.Z);
                    //cell_particle_number (i,j)에는 0.5 빼주고, (i,j+1)에는 0.5 더해주기
                    cell_particle_number->cell_values[tool->get_VectorIndex_from_cell(i_j_k)] = cell_particle_number->cell_values[tool->get_VectorIndex_from_cell(i_j_k)] - 0.5;
                    cell_particle_number->cell_values[tool->get_VectorIndex_from_cell(i_j_plus_1_k)] = cell_particle_number->cell_values[tool->get_VectorIndex_from_cell(i_j_plus_1_k)] + 0.5;

                    //previous_velocity_grid 에다가 각각 velocity의 0.5배씩 더해주기
                    previous_velocity_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_k)] = previous_velocity_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_k)] + particles[p].Velocity * 0.5;
                    previous_velocity_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_plus_1_k)] = previous_velocity_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_plus_1_k)] + particles[p].Velocity * 0.5;
                }

                //3-2. j면과 가까움
                else if (check == 2) {
                    Vector3D i_j_minus_1_k = Vector3D(i_j_k.X, i_j_k.Y - 1, i_j_k.Z);
                    //cell_particle_number (i,j)에는 0.5 빼주고, (i,j-1)에는 0.5 더해주기
                    cell_particle_number->cell_values[tool->get_VectorIndex_from_cell(i_j_k)] = cell_particle_number->cell_values[tool->get_VectorIndex_from_cell(i_j_k)] - 0.5;
                    cell_particle_number->cell_values[tool->get_VectorIndex_from_cell(i_j_minus_1_k)] = cell_particle_number->cell_values[tool->get_VectorIndex_from_cell(i_j_minus_1_k)] + 0.5;

                    //previous_velocity_grid 에다가 각각 velocity의 0.5배씩 더해주기
                    previous_velocity_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_k)] = previous_velocity_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_k)] + particles[p].Velocity * 0.5;
                    previous_velocity_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_minus_1_k)] = previous_velocity_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_minus_1_k)] + particles[p].Velocity * 0.5;
                }
                //3-3. i_pus면과 가까움
                else if (check == 3) {
                    Vector3D i_plus_1_j_k = Vector3D(i_j_k.X + 1, i_j_k.Y, i_j_k.Z);
                    //cell_particle_number (i,j)에는 0.5 빼주고, (i+1,j)에는 0.5 더해주기
                    cell_particle_number->cell_values[tool->get_VectorIndex_from_cell(i_j_k)] = cell_particle_number->cell_values[tool->get_VectorIndex_from_cell(i_j_k)] - 0.5;
                    cell_particle_number->cell_values[tool->get_VectorIndex_from_cell(i_plus_1_j_k)] = cell_particle_number->cell_values[tool->get_VectorIndex_from_cell(i_plus_1_j_k)] + 0.5;

                    //previous_velocity_grid 에다가 각각 velocity의 0.5배씩 더해주기
                    previous_velocity_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_k)] = previous_velocity_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_k)] + particles[p].Velocity * 0.5;
                    previous_velocity_grid->cell_values[tool->get_VectorIndex_from_cell(i_plus_1_j_k)] = previous_velocity_grid->cell_values[tool->get_VectorIndex_from_cell(i_plus_1_j_k)] + particles[p].Velocity * 0.5;
                }

                //3-4. i면과 가까움
                else if (check==4) { 
                    Vector3D i_minus_1_j_k = Vector3D(i_j_k.X - 1, i_j_k.Y, i_j_k.Z);
                    //cell_particle_number (i,j)에는 0.5 빼주고, (i-1,j)에는 0.5 더해주기
                    cell_particle_number->cell_values[tool->get_VectorIndex_from_cell(i_j_k)] = cell_particle_number->cell_values[tool->get_VectorIndex_from_cell(i_j_k)] - 0.5;
                    cell_particle_number->cell_values[tool->get_VectorIndex_from_cell(i_minus_1_j_k)] = cell_particle_number->cell_values[tool->get_VectorIndex_from_cell(i_minus_1_j_k)] + 0.5;

                    //previous_velocity_grid 에다가 각각 velocity의 0.5배씩 더해주기
                    previous_velocity_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_k)] = previous_velocity_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_k)] + particles[p].Velocity * 0.5;
                    previous_velocity_grid->cell_values[tool->get_VectorIndex_from_cell(i_minus_1_j_k)] = previous_velocity_grid->cell_values[tool->get_VectorIndex_from_cell(i_minus_1_j_k)] + particles[p].Velocity * 0.5;
                }

                //3-5.  k_plus 면과 가까움
                else if (check==5) { 
                    Vector3D i_j_k_plus_1 = Vector3D(i_j_k.X, i_j_k.Y, i_j_k.Z+1);
                    //cell_particle_number (i,j)에는 0.5 빼주고, (i+1,j)에는 0.5 더해주기
                    cell_particle_number->cell_values[tool->get_VectorIndex_from_cell(i_j_k)] = cell_particle_number->cell_values[tool->get_VectorIndex_from_cell(i_j_k)] - 0.5;
                    cell_particle_number->cell_values[tool->get_VectorIndex_from_cell(i_j_k_plus_1)] = cell_particle_number->cell_values[tool->get_VectorIndex_from_cell(i_j_k_plus_1)] + 0.5;

                    //previous_velocity_grid 에다가 각각 velocity의 0.5배씩 더해주기
                    previous_velocity_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_k)] = previous_velocity_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_k)] + particles[p].Velocity * 0.5;
                    previous_velocity_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_k_plus_1)] = previous_velocity_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_k_plus_1)] + particles[p].Velocity * 0.5;
                }

                //3-6.  k면과 가까움
                else {
                    Vector3D i_j_k_minus_1 = Vector3D(i_j_k.X + 1, i_j_k.Y, i_j_k.Z-1);
                    //cell_particle_number (i,j)에는 0.5 빼주고, (i+1,j)에는 0.5 더해주기
                    cell_particle_number->cell_values[tool->get_VectorIndex_from_cell(i_j_k)] = cell_particle_number->cell_values[tool->get_VectorIndex_from_cell(i_j_k)] - 0.5;
                    cell_particle_number->cell_values[tool->get_VectorIndex_from_cell(i_j_k_minus_1)] = cell_particle_number->cell_values[tool->get_VectorIndex_from_cell(i_j_k_minus_1)] + 0.5;

                    //previous_velocity_grid 에다가 각각 velocity의 0.5배씩 더해주기
                    previous_velocity_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_k)] = previous_velocity_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_k)] + particles[p].Velocity * 0.5;
                    previous_velocity_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_k_minus_1)] = previous_velocity_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_k_minus_1)] + particles[p].Velocity * 0.5;
                }
            }

        }


        //4. cell에 포함된 particle의 속도 평균내기
        for (int i = 0; i < cell_number; i++) {
            if (cell_particle_number->cell_values[i] != 0) {
                previous_velocity_grid->cell_values[i] = previous_velocity_grid->cell_values[i] / cell_particle_number->cell_values[i];
            }
            else {
                previous_velocity_grid->cell_values[i] = Vector3D();
            }
        }

        //5. cell에 포함된 속도로 velocity_difference_grid 값 채우기
        for (int i = 0; i < gridsize; i++) {
            for (int j = 0; j < gridsize; j++) {
                for (int k = 0; k < gridsize; k++) {
                    //
                    Vector3D i_j_k = Vector3D(i, j, k);

                    Vector3D i_plus1_j_k = Vector3D(i + 1, j,k);
                    Vector3D i_j_plus1_k = Vector3D(i, j + 1,k);
                    Vector3D i_j_k_plus1 = Vector3D(i, j, k + 1);

                    //발산 구하기 - 속도의 공간 변화량 구하기
                    velocity_difference_X_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_k)] = (previous_velocity_grid->cell_values[tool->get_VectorIndex_from_cell(i_plus1_j_k)].X - previous_velocity_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_k)].X);
                    if (i == 0) {
                        velocity_difference_X_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_k)] = previous_velocity_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_k)].X;
                    }
                    velocity_difference_Y_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_k)] = (previous_velocity_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_plus1_k)].Y - previous_velocity_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_k)].Y);
                    if (j == 0) {
                        velocity_difference_Y_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_k)] = previous_velocity_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_k)].Y;
                    }
                    velocity_difference_Z_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_k)] = (previous_velocity_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_k_plus1)].Z - previous_velocity_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_k)].Z);
                    if (j == 0) {
                        velocity_difference_Z_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_k)] = previous_velocity_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_k)].Z;
                    }
                }
            }
        }
    }

    void transfer_Velocity_to_particle_from_grid() {
        for (int p = 0; p < particles.size(); p++) {
            //1. particle이 속한 cell 찾기
            Vector3D i_j_k = tool->get_cell_i_j_k_from_world(particles[p].Location);

            //2. cell 정보를 particle에게 전달
            particles[p].Velocity = next_velocity_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_k)];

            //난수 준비
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<int> theta(0, 360);

            //3. 랜덤 벡터 생성 - 크기 1
            int angle_theta = theta(gen);
            int angle_phi = theta(gen);
            Vector3D randomvector = Vector3D(cos(angle_theta) * sin(angle_phi), sin(angle_theta) * sin(angle_phi),cos(angle_theta) );

            double scale = sqrt(pow(particles[p].Velocity.X, 2) + pow(particles[p].Velocity.Y, 2)) * 0.05;

            particles[p].Velocity = particles[p].Velocity + (randomvector * scale);

        }
    }

    //void transfer_Velocity_to_particle_from_grid_PICFLIP(double PIC_ratio, double FLIP_ratio) {
    //    //0. cell에 포함된 속도로 velocity_difference_grid 값 update하기
    //    for (int i = 0; i < gridsize; i++) {
    //        for (int j = 0; j < gridsize; j++) {
    //            //
    //            Vector2D i_j = Vector2D(i, j);

    //            Vector2D i_plus1_j = Vector2D(i + 1, j);
    //            Vector2D i_j_plus1 = Vector2D(i, j + 1);

    //            //발산 구하기 - 속도의 공간 변화량 구하기
    //            velocity_difference_X_grid->cell_values[next_velocity_grid->get_VectorIndex_from_cell(i_j)] = (next_velocity_grid->cell_values[next_velocity_grid->get_VectorIndex_from_cell(i_plus1_j)].X - next_velocity_grid->cell_values[next_velocity_grid->get_VectorIndex_from_cell(i_j)].X);
    //            if (i == 0) {
    //                velocity_difference_X_grid->cell_values[next_velocity_grid->get_VectorIndex_from_cell(i_j)] = next_velocity_grid->cell_values[next_velocity_grid->get_VectorIndex_from_cell(i_j)].X;
    //            }
    //            velocity_difference_Y_grid->cell_values[next_velocity_grid->get_VectorIndex_from_cell(i_j)] = (next_velocity_grid->cell_values[next_velocity_grid->get_VectorIndex_from_cell(i_j_plus1)].Y - next_velocity_grid->cell_values[next_velocity_grid->get_VectorIndex_from_cell(i_j)].Y);
    //            if (j == 0) {
    //                velocity_difference_Y_grid->cell_values[next_velocity_grid->get_VectorIndex_from_cell(i_j)] = next_velocity_grid->cell_values[next_velocity_grid->get_VectorIndex_from_cell(i_j)].Y;
    //            }
    //        }
    //    }

    //    for (int p = 0; p < particles.size(); p++) {
    //        //1. particle이 속한 cell 찾기
    //        Vector2D i_j = next_velocity_grid->get_cell_i_j_from_world(particles[p].Location);

    //        //2. PIC
    //        Vector2D PIC_velocity = next_velocity_grid->cell_values[next_velocity_grid->get_VectorIndex_from_cell(i_j)];

    //        //3. FLIP
    //        Vector2D FLIP_velocity = Vector2D();
    //        FLIP_velocity.X = PIC_velocity.X + (previous_velocity_grid->cell_values[previous_velocity_grid->get_VectorIndex_from_cell(i_j)].X);
    //        FLIP_velocity.Y = PIC_velocity.Y + (previous_velocity_grid->cell_values[previous_velocity_grid->get_VectorIndex_from_cell(i_j)].Y);

    //        //4. cell 정보를 PIC-FLIP 방법에 따라 particle에게 전달
    //        particles[p].Velocity = PIC_velocity * PIC_ratio + FLIP_velocity * FLIP_ratio;

    //        //난수 준비
    //        std::random_device rd;
    //        std::mt19937 gen(rd());
    //        std::uniform_int_distribution<int> theta(0, 360);

    //        //5. 랜덤 벡터 생성 - 크기 1
    //        int angle = theta(gen);
    //        Vector2D randomvector = Vector2D(cos(angle), sin(angle));

    //        double scale = sqrt(pow(particles[p].Velocity.X, 2) + pow(particles[p].Velocity.Y, 2)) * 0.10;

    //        particles[p].Velocity = particles[p].Velocity + (randomvector * scale);

    //    }
    //}

    //유체 속도값을 인접 공기셀에 보간
    void extrapolate_velocity_to_air_cell() {
        //air_cell_center_point 초기화
        air_cell_center_point->clear();

        for (int i = 0; i < cell_number; i++) {
            //1. i_j 선언
            Vector3D i_j_k = Vector3D(tool->get_cell_i_from_VectorIndex(i), tool->get_cell_j_from_VectorIndex(i), tool->get_cell_k_from_VectorIndex(i));

            //2. i_j 셀이 공기셀이라면
            if (cell_type_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_k)] == AIR) {
                int n_fluid_cells = 0;

                Vector3D i_plus1_j_k = Vector3D(i_j_k.X + 1, i_j_k.Y, i_j_k.Z);
                Vector3D i_minus1_j_k = Vector3D(i_j_k.X - 1, i_j_k.Y, i_j_k.Z);
                Vector3D i_j_plus1_k = Vector3D(i_j_k.X, i_j_k.Y + 1, i_j_k.Z);
                Vector3D i_j_minus1_k = Vector3D(i_j_k.X, i_j_k.Y - 1, i_j_k.Z);
                Vector3D i_j_k_plus1 = Vector3D(i_j_k.X, i_j_k.Y, i_j_k.Z + 1);
                Vector3D i_j_k_minus1 = Vector3D(i_j_k.X, i_j_k.Y, i_j_k.Z - 1);

                //3. 인접한 유체셀의 속도를 더한다.
                //3-1. i+1, j
                if (cell_type_grid->cell_values[tool->get_VectorIndex_from_cell(i_plus1_j_k)] == FLUID) {
                    previous_velocity_grid->cell_values[i] = previous_velocity_grid->cell_values[i] + previous_velocity_grid->cell_values[tool->get_VectorIndex_from_cell(i_plus1_j_k)];
                    n_fluid_cells++;
                }
                //3-2. i-1, j
                if (cell_type_grid->cell_values[tool->get_VectorIndex_from_cell(i_minus1_j_k)] == FLUID) {
                    previous_velocity_grid->cell_values[i] = previous_velocity_grid->cell_values[i] + previous_velocity_grid->cell_values[tool->get_VectorIndex_from_cell(i_minus1_j_k)];
                    n_fluid_cells++;
                }
                //3-3. i, j+1
                if (cell_type_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_plus1_k)] == FLUID) {
                    previous_velocity_grid->cell_values[i] = previous_velocity_grid->cell_values[i] + previous_velocity_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_plus1_k)];
                    n_fluid_cells++;
                }
                //3-4. i, j-1
                if (cell_type_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_minus1_k)] == FLUID) {
                    previous_velocity_grid->cell_values[i] = previous_velocity_grid->cell_values[i] + previous_velocity_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_minus1_k)];
                    n_fluid_cells++;
                }

                //3-5. k+1
                if (cell_type_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_k_plus1)] == FLUID) {
                    previous_velocity_grid->cell_values[i] = previous_velocity_grid->cell_values[i] + previous_velocity_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_k_plus1)];
                    n_fluid_cells++;
                }

                //3-6. k-1
                if (cell_type_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_k_minus1)] == FLUID) {
                    previous_velocity_grid->cell_values[i] = previous_velocity_grid->cell_values[i] + previous_velocity_grid->cell_values[tool->get_VectorIndex_from_cell(i_j_k_minus1)];
                    n_fluid_cells++;
                }

                //4. 인접한 유체셀의 속도의 평균을 구한다.
                if (n_fluid_cells > 0) {
                    //속도 평균내기
                    previous_velocity_grid->cell_values[i] = previous_velocity_grid->cell_values[i] / (double)n_fluid_cells;

                    //extrapolation 받은 cell을 air_cell에 넣는다.
                    air_cell_center_point->push_back(cell_center_point->cell_values[i]);
                }

            }
        }
    }

    //==============================cell type 분류==================================

    void classify_cell_type() {
        for (int i = 0; i < cell_number; i++) {
            //cell 좌표 구하기
            Vector3D i_j_k = Vector3D(tool->get_cell_i_from_VectorIndex(i), tool->get_cell_j_from_VectorIndex(i), tool->get_cell_k_from_VectorIndex(i));

            //셀이 벽쪽이라면 solid
            if (i_j_k.X == 0 || i_j_k.X == gridsize - 1 || i_j_k.Y == 0 || i_j_k.Y == gridsize - 1 || i_j_k.Z == 0 || i_j_k.Z == gridsize - 1) {
                cell_type_grid->cell_values[i] = SOLID;
            }
            else {
                //particle이 존재하는 cell이라면 fluid
                if (cell_particle_number->cell_values[i] > 0) {
                    cell_type_grid->cell_values[i] = FLUID;
                }
                //particle이 존재하지 않는 cell이라면 air
                else {
                    cell_type_grid->cell_values[i] = AIR;
                }
            }
        }
    }


    //==============================================================================

    //==============================bodyforce 관련 함수==============================

    //bodyforce에 적용하고 싶은 외력 추가 - 중력
    void add_body_force() {
        for (int i = 0; i < cell_number; i++) {
            bodyforce->cell_values[i] = Vector3D(0.0, -9.8, 0.0);
        }
    }

    //bodyforce를 grid의 velocity에 적용
    void Adjust_velocity_from_bodyforce() {
        for (int i = 0; i < cell_number; i++) {
            previous_velocity_grid->cell_values[i] = previous_velocity_grid->cell_values[i] + bodyforce->cell_values[i] * timestep;
        }
    }

    ////일괄적으로 같은 값을 전달받아도 되지만, 일단 포함된 cell의 정보를 받을 수 있게 해놓음.
    //void transfer_bodyforce_to_particle_from_grid() {
    //    for (int p = 0; p < particles.size(); p++) {
    //        //1. particle이 어디 cell인지 파악
    //        Vector2D i_j = bodyforce.get_cell_i_j_from_world(particles[p].Location);

    //        //2. bodyforce cell값을 Accceleration에 전달
    //        particles[p].Acceleration = bodyforce.cell_values[bodyforce.get_VectorIndex_from_cell(i_j)];
    //    }
    //}

    //==============================================================================


    //==============================pressure_solve=================================
    void pressure_solve() {

        //solver
        Eigen::BiCGSTAB<Eigen::SparseMatrix<double>> cg_solver;

        cg_solver.setMaxIterations(150);

        //0. 압력 계수 및 행렬 선언
        double pressure_coefficient = timestep / density;

        Eigen::VectorXd x, b, b1;
        x = Eigen::VectorXd(cell_number);
        b = Eigen::VectorXd(cell_number);

        //올바른 참조를 위해 0으로 초기화
        A->setZero();

        //1.Ax=b에서 A 작성 - diagonal 요소들도 작성
        for (int i = 0; i < cell_number; i++) {
            double neighbor_sum = 0.0;
            Vector3D i_j_k = Vector3D(tool->get_cell_i_from_VectorIndex(i), tool->get_cell_j_from_VectorIndex(i), tool->get_cell_k_from_VectorIndex(i));

            if (cell_particle_number->cell_values[i] == 0) { continue; }
            else {
                //현재 셀의 좌표 구하기
                Vector3D i_minus1_j_k = i_j_k + Vector3D(-1, 0,0);
                Vector3D i_plus1_j_k = i_j_k + Vector3D(1, 0,0);
                Vector3D i_j_minus1_k = i_j_k + Vector3D(0, -1,0);
                Vector3D i_j_plus1_k = i_j_k + Vector3D(0, 1,0);
                Vector3D i_j_k_minus1 = i_j_k + Vector3D(0, 0, -1);
                Vector3D i_j_k_plus1 = i_j_k + Vector3D(0, 0, 1);

                //셀좌표에 대응되는 vectorIndex좌표 구하기

                //i-1,j
                if (i_j_k.X > 0) {
                    int cell_i_minus1_j_k = tool->get_VectorIndex_from_cell(i_minus1_j_k);
                    if (cell_particle_number->cell_values[cell_i_minus1_j_k] > 0) {
                        A->insert(i, cell_i_minus1_j_k) -= pressure_coefficient / (delta_x * delta_x);
                        A->insert(cell_i_minus1_j_k, i) -= pressure_coefficient / (delta_x * delta_x);
                    }
                }
                //i+1,j
                if (i_j_k.X < gridsize - 1) {
                    int cell_i_plus1_j_k = tool->get_VectorIndex_from_cell(i_plus1_j_k);
                    if (cell_particle_number->cell_values[cell_i_plus1_j_k] > 0) {
                        A->insert(i, cell_i_plus1_j_k) -= pressure_coefficient / (delta_x * delta_x);
                        A->insert(cell_i_plus1_j_k, i) -= pressure_coefficient / (delta_x * delta_x);
                    }
                }
                //i,j-1
                if (i_j_k.Y > 0) {
                    int cell_i_j_minus1_k = tool->get_VectorIndex_from_cell(i_j_minus1_k);
                    if (cell_particle_number->cell_values[cell_i_j_minus1_k] > 0) {
                        A->insert(i, cell_i_j_minus1_k) -= pressure_coefficient / (delta_x * delta_x);
                        A->insert(cell_i_j_minus1_k, i) -= pressure_coefficient / (delta_x * delta_x);
                    }
                }
                //i,j+1
                if (i_j_k.Y < gridsize -1) {
                    int cell_i_j_plus1_k = tool->get_VectorIndex_from_cell(i_j_plus1_k);
                    if (cell_particle_number->cell_values[cell_i_j_plus1_k] > 0) {
                        A->insert(i, cell_i_j_plus1_k) -= pressure_coefficient / (delta_x * delta_x);
                        A->insert(cell_i_j_plus1_k, i) -= pressure_coefficient / (delta_x * delta_x);
                    }
                }
                //k-1
                if (i_j_k.Z > 0) {
                    int cell_i_j_k_minus1 = tool->get_VectorIndex_from_cell(i_j_k_minus1);
                    if (cell_particle_number->cell_values[cell_i_j_k_minus1] > 0) {
                        A->insert(i, cell_i_j_k_minus1) -= pressure_coefficient / (delta_x * delta_x);
                        A->insert(cell_i_j_k_minus1, i) -= pressure_coefficient / (delta_x * delta_x);
                    }
                }
                //k+1
                if (i_j_k.Z < gridsize - 1) {
                    int cell_i_j_k_plus1 = tool->get_VectorIndex_from_cell(i_j_k_plus1);
                    if (cell_particle_number->cell_values[cell_i_j_k_plus1] > 0) {
                        A->insert(i, cell_i_j_k_plus1) -= pressure_coefficient / (delta_x * delta_x);
                        A->insert(cell_i_j_k_plus1, i) -= pressure_coefficient / (delta_x * delta_x);
                    }
                }
            }

        }

        //diagonal 요소들도 작성
        for (int i = 0; i < cell_number; i++) {
            A->insert(i, i) = 6 * pressure_coefficient / (delta_x * delta_x);
        }

        //      //작은 양 검출

        //      cout << "=====================================================" << endl;
        //     
        //      for (int i = 0; i < gridsize; i++) {
        //          for (int j = 0; j < gridsize; j++) {
        //              cout << A->coeff(i,j) << " ";
        //          }
        //          cout << endl;
        //      }

        //      cout << "=====================================================" << endl;

        //      cout << "b is " << endl;
        //      for (int i = 0; i < 40; i++) {
        //          if (i + 80 < 100) {
        //              cout << b(i) << "             " << "////" << " " << b(i + 40) << " ////" << "                " << b(i + 80) << endl; //0~39, 40~79
        //          }

        //          else {
        //              cout << b(i) << "             " << "////" << " " << b(i + 40) << endl;
        //          }

              //}

        //      cout << "=====================================================" << endl;


              ////2. Ax=b에서 b 작성 - 벡터의 발산 - 그냥 요소를 다 더하자.
              //for (int i = 0; i < cell_number; i++) {
              //    b(i) = previous_velocity_grid.cell_values[i].X + previous_velocity_grid.cell_values[i].Y;
              //}

              //2. Ax=b에서 b 작성 - 벡터의 발산 - 제대로...?
        for (int k = 0; k < cell_number; k++) {
            b(k) = -1.0 * ( velocity_difference_X_grid->cell_values[k] + velocity_difference_Y_grid->cell_values[k] + velocity_difference_Z_grid->cell_values[k] );
        }

        //3. x solve
        cg_solver.compute(*A);
        x = cg_solver.solve(b);

        std::cout << "#iterations:     " << cg_solver.iterations() << std::endl;
        std::cout << "estimated error: " << cg_solver.error() << std::endl;

        //cout << "=====================================================" << endl;

        //cout << "x is " << endl;
        //for (int i = 0; i < 40; i++) {
        //    if (i + 80 < 100) {
        //        cout << x(i) << "             " << "////" << " " << x(i + 40) << " ////" << "                " << x(i + 80) << endl; //0~39, 40~79
        //    }

        //    else {
        //        cout << x(i) << "             " << "////" << " " << x(i + 40) << endl;
        //    }

        //}

        //cout << "=====================================================" << endl;

        ////cout << A << endl;
        //cout << "===========================================" << endl;
        //cout << x << endl;
        //cout << "===========================================" << endl;    
        //cout << b << endl;

        ///* ... update b ... */
        //x = cg_solver.solve(b); // solve again

        ////cout << A << endl;
        //cout << "===========================================" << endl;
        //cout << x << endl;
        //cout << "===========================================" << endl;
        //cout << b << endl;

        cout << " solver_done " << endl;

        //4. x 값( 압력 )을 통해 cell들의 속도 계산
        for (int i = 0; i < cell_number; i++) {
            //4-1. 새로운 속도 계산 - 압력 그대로 사용 - PIC
            double new_vel_x = previous_velocity_grid->cell_values[i].X - timestep / density * x(i);
            double new_vel_y = previous_velocity_grid->cell_values[i].Y - timestep / density * x(i);
            double new_vel_z = previous_velocity_grid->cell_values[i].Z - timestep / density * x(i);


            //4-2. new vel에다가 입력하기
            next_velocity_grid->cell_values[i] = Vector3D(new_vel_x, new_vel_y, new_vel_z);
        }


        //메모리 반납


    }


    //==============================================================================


    //==============================swap buffer=================================

    //previous와 next_velocity 맞바꾸기


    void swap_buffer() {
        MAC_Grid<Vector3D>* tmp = new MAC_Grid<Vector3D>(gridsize);
        *tmp = *previous_velocity_grid;
        *previous_velocity_grid = *next_velocity_grid;
        *next_velocity_grid = *tmp;

        delete tmp;

    }

    //==============================================================================

    //==============================rendering fluid================================

    void rendering_fluid() {
        //1. fluid center grid clear()
        fluid_cell_center_point->clear();

        for (int i = 0; i < cell_number; i++) {
            //2. cellType이 fluid인 cell들의 center point를 구한다.
            if (cell_type_grid->cell_values[i] == FLUID) {
                fluid_cell_center_point->push_back(cell_center_point->cell_values[i]);
            }
        }
    }


    //=============================================================================

};

