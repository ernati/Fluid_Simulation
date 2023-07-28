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

class Fluid_Simulator_Grid {

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


    Fluid_Simulator_Grid() {
        //timestep의 default = 0.06
        timestep = 0.06;

        gridsize = 10;

        cellsize = 1.0 / gridsize;

        //velocity grid 초기화
        velocity_grid = MAC_Grid<Vector2D>(gridsize);
        for (int n = 0; n < (gridsize) * (gridsize); n++) {
            velocity_grid.cell_values.push_back(Vector2D());
        }
    }

    Fluid_Simulator_Grid(int particle_number, int grid_N) {

        //timestep의 default = 0.06
        timestep = 0.06;

        gridsize = grid_N;

        cellsize = 1.0 / gridsize;

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
            bodyforce.cell_values[i] = Vector2D(0.0, -0.000000098);
        }
    }

    //bodyforce를 velocity grid에 적용 - 등가속도
    void apply_bodyforce_to_Velocity() {
        for (int i = 0; i < (gridsize * gridsize); i++) {

            //if ( i == 200 ) std::cout << velocity_grid.cell_values[i].X << " " << velocity_grid.cell_values[i].Y << endl;
            velocity_grid.cell_values[i] = velocity_grid.cell_values[i] + bodyforce.cell_values[i] * timestep;
            //if( i == 200 ) std::cout << velocity_grid.cell_values[i].X << " " << velocity_grid.cell_values[i].Y << endl;
            //if( i == 200 )std::cout << endl;
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
        for (int p = 0; p < particles.size(); p++) {
            //1. particle cell좌표 찾기
            Vector2D i_j = velocity_grid.get_cell_i_j_from_world(particles[p].Location);

            //2. velocity cell값을 주변 grid로 전달
            extrapolation_value_to_grid_from_particle(velocity_grid, particles[p].Location, velocity_grid.cell_values[velocity_grid.get_VectorIndex_from_cell(i_j)]);

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
        for (int p = 0; p < particles.size(); p++) {
            //1. particle이 속한 cell 찾기
            Vector2D i_j = velocity_grid.get_cell_i_j_from_world(particles[p].Location);

            //2. cell 정보를 particle에게 전달
            particles[p].Velocity = interpolate_value_to_particle_from_grid(velocity_grid, particles[p].Location);
        }
    }



};

class Fluid_Simulator_OnlyParticle {
public:

    //particle들을 담을 vector
    vector<Particle2D> particles;
    float timestep;
    float density;

    float delta_x;
    float delta_y;

    //particle들의 속도를 담을 vector
    vector<float> previous_velocity_x;
    vector<float> previous_velocity_y;

    vector<float> next_velocity_x;
    vector<float> next_velocity_y;

    //아무 입력값이 없다면 기본 particle 수는 100개, grid_N = 10
    Fluid_Simulator_OnlyParticle() {

        //입자들 초기화
        srand((unsigned int)time(NULL));
        for (int i = 0; i < 100; i++) {
            //0~99 난수 생성    
            int randomLocation_X = rand() % 100;
            //0~99 난수 생성
            int randomLocation_Y = rand() % 100;

            //위치 X : 0.1~ 0.2, 위치 Y : 0.8 ~ 0.9
            Particle2D tmp = Particle2D(((float)randomLocation_X + 100.0) / 1000.0, ((float)randomLocation_X + 800.0) / 1000.0, (float)randomLocation_X / 500.0, (float)randomLocation_Y / 500.0, 0, -0.98);
            particles.push_back(tmp);

            previous_velocity_x.push_back(tmp.Velocity.X);
            previous_velocity_y.push_back(tmp.Velocity.Y);

            next_velocity_x.push_back(0.0);
            next_velocity_y.push_back(0.0);
        }

        //timestep의 default = 0.06
        timestep = 0.06;
        density = 1.0;
        delta_x = 1.0 / 10.0;
        delta_y = 1.0 / 10.0;
    }

    Fluid_Simulator_OnlyParticle(int particle_number, int grid_N) {

        //입자들 초기화
        srand((unsigned int)time(NULL));
        for (int i = 0; i < particle_number; i++) {
            //0~99 난수 생성    
            int randomLocation_X = rand() % 100;
            //0~99 난수 생성
            int randomLocation_Y = rand() % 100;

            //위치 X : 0.1~ 0.2, 위치 Y : 0.8 ~ 0.9
            Particle2D tmp = Particle2D( ( (float)randomLocation_X + 100.0) / 1000.0, ((float)randomLocation_X + 800.0) / 1000.0, (float)randomLocation_X / 500.0 + 0.1, (float)randomLocation_Y / 500.0, 0, -0.98);
            particles.push_back(tmp);

            previous_velocity_x.push_back(tmp.Velocity.X);
            previous_velocity_y.push_back(tmp.Velocity.Y);

            next_velocity_x.push_back(0.0);
            next_velocity_y.push_back(0.0);
        }

        //timestep의 default = 0.06
        timestep = 0.06;
        density = 1.0;
        delta_x = 1.0 / (float)grid_N;
        delta_y = 1.0 / (float)grid_N;
    }

    void particle_simulation() {

        cout << " boundarycondition start " << endl;

        //1. 경계 처리
        boundarycondition();

        cout << " advection start " << endl;

        //2. advection
        advection();

        cout << " bodyforce start " << endl;

        //3. bodyforce 적용
        bodyforce();

        cout << " pressureSolve start " << endl;

        //4. 압력 계산
        pressure_solve(1.0);

        //5. 추가적인 상호작용
        //...

        cout << " Update particle Velocity start " << endl;

        //6. 상호 작용에 따른 속도 계산
        Update_particles_Velocity();

        cout << " Swap buffers start " << endl;

        //7. new와 previous 자리를 바꾼다.
        swap_buffers();
    }

    //속도에 따른 위치 예상 결과값이 boundarycondition을 벗어나는 경우 속도를 적당한 조건으로 변경함.
    void boundarycondition() {
        for (int i = 0; i < particles.size(); i++) {
            Vector2D expecting_Location = this->particles[i].Location + this->particles[i].Velocity * timestep;

            //boundary를 벗어나면, particle의 위치를 옮기고, 속도를 0으로 초기화.
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
        particle.Location.X = 0.15;
        particle.Velocity.X = 0.0;

        particle.Location.Y = 0.0;
        particle.Velocity.Y = 0.0;
    }

    void advection() {
        Update_particles_Location();
    }

    //particle의 속도를 previous vector로 옮김
    void Update_Velocity_particle_to_vector() {
        for (int i = 0; i < particles.size(); i++) {
            previous_velocity_x[i] = particles[i].Velocity.X;
            previous_velocity_y[i] = particles[i].Velocity.Y;
        }
    }

    //next vector의 속도를 particle로 옮김
    void Update_Velocity_vector_to_particle() {
        for (int i = 0; i < particles.size(); i++) {
            Vector2D vel = Vector2D(next_velocity_x[i], next_velocity_y[i]);
            particles[i].Velocity = vel;
        }
    }


    //입자들의 속도를 모두 update
    void Update_particles_Velocity() {
        //1. vector의 속도를 particle로 옮김
        Update_Velocity_vector_to_particle();
    }

    //입자들의 위치를 모두 update
    void Update_particles_Location() {
        for (int i = 0; i < particles.size(); i++) {
            this->particles[i].Location = this->particles[i].Location + this->particles[i].Velocity * timestep;
        }
    }

    void bodyforce() {
        //1. particle에 가속도 적용
        for (int i = 0; i < particles.size(); i++) {
            this->particles[i].Acceleration.X = 0.0;
            this->particles[i].Acceleration.Y = -0.98;
        }

        //2. 가속도로 particle의 속도 계산 -> u = u + tg
        for (int i = 0; i < previous_velocity_y.size(); i++) {
            previous_velocity_y[i] = previous_velocity_y[i] + timestep * -0.98;
        }
    }

    void pressure_solve(float threshold_distance) {


        //0. 압력 계수 및 행렬 선언
        float pressure_coefficient = timestep / density;

        int n_particles = particles.size();
        //Eigen::Matrix< float, Eigen::Dynamic, Eigen::Dynamic > A;
        ////Eigen::SparseMatrix<float> A;
        ////Eigen::Matrix< float, Eigen::Dynamic, 1 > x;
        //Eigen::VectorXd x(n_particles);
        //Eigen::Matrix< float, Eigen::Dynamic, 1 > b;

        Eigen::VectorXd x(n_particles), b(n_particles);

        Eigen::SparseMatrix<double> A(n_particles, n_particles);
        //x.resize(n_particles, 1);
        //b.resize(n_particles, 1);

        //A.Zero();
        //x.Zero();
        //b.Zero();

        //solver
        Eigen::BiCGSTAB<Eigen::SparseMatrix<double>> cg_solver;

        //1. Ax=b에서 A 작성 - 입자 간의 거리 측정
        for (int i = 0; i < particles.size(); i++) {
            for (int j = 0; j < particles.size(); j++) {
                // 1. ( i,j )의 계수 
                if (i < j) {
                    // 1-1. 거리 측정
                    float distance = particles[i].get_distance_from_point(particles[j].Location);

                    // 1-2. 계수 작성, 반대편 위치도 작성
                    if (distance > threshold_distance) { A.insert(i, j) = 0.0; A.insert(j, i) = 0.0; }
                    else { A.insert(i, j) = pressure_coefficient / (distance * distance);  A.insert(j, i) = pressure_coefficient / (distance * distance); }

                }
            }
        }

        //diagonal 요소들도 작성
        for (int i = 0; i < particles.size(); i++) {
            A.insert(i, i) = 1.0;
        }

        //2. Ax=b에서 b 작성 - 벡터의 발산 - 그냥 요소를 다 더하자.
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

        //4. x 값( 압력 )을 통해 입자들의 particle 계산
        for (int i = 0; i < previous_velocity_x.size(); i++) {
            //4-1. 새로운 속도 계산 - 압력 그대로 사용
            float new_vel_x = previous_velocity_x[i] - timestep / density * x(i);
            float new_vel_y = previous_velocity_y[i] - timestep / density * x(i);

            //4-2. new vel에다가 입력하기
            next_velocity_x[i] = new_vel_x;
            next_velocity_y[i] = new_vel_y;
        }

    }

    void swap_buffers() {
        vector<float> tmp_x;
        vector<float> tmp_y;
        
        //1. new를 tmp에 모두 담음
        for (int i = 0; i < next_velocity_x.size(); i++) {
            tmp_x.push_back(next_velocity_x[i]);
            tmp_y.push_back(next_velocity_y[i]);
        }

        //2. new에 previous를 모두 담음
        for (int i = 0; i < previous_velocity_x.size(); i++) {
            next_velocity_x[i] = previous_velocity_x[i];
            next_velocity_y[i] = previous_velocity_y[i];
        }

        //3. previous에 tmp를 담는다.
        for (int i = 0; i < tmp_x.size(); i++) {
            previous_velocity_x[i] = tmp_x[i];
            previous_velocity_y[i] = tmp_y[i];
        }
    }

};
