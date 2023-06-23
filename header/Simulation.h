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

public : 

    //particle들을 담을 vector
    vector<Particle2D> particles;
    float timestep;

    MAC_Grid<Vector2D> velocity_grid;


    //아무 입력값이 없다면 기본 particle 수는 100개, grid_N = 10
    Constant_Acceleration_Simulator() {

        //입자들 초기화
        srand((unsigned int)time(NULL));
        for(int i = 0; i<100; i++) {
            //0~99 난수 생성    
            int randomLocation_X = rand() % 100 ;
            //0~49 난수 생성
            int randomLocation_Y = rand() % 50 ;

            //위치 X : 0.0~ 0.99, 위치 Y : 0.5 ~ 0.99
            Particle2D tmp = Particle2D( (float)randomLocation_X / 100.0 ,  (float)randomLocation_Y / 100.0 + 0.5 , 0,0,0,-0.0098 );
            particles.push_back( tmp );
        }

        //velocity grid 그리기
        velocity_grid = MAC_Grid<Vector2D>(10);

        //timestep의 default = 0.06
        timestep = 0.06;
    }

    Constant_Acceleration_Simulator( int particle_number, int grid_N ) {

        //입자들 초기화
        srand((unsigned int)time(NULL));
        for(int i = 0; i<particle_number; i++) {
            //0~99 난수 생성    
            int randomLocation_X = rand() % 100 ;
            //0~49 난수 생성
            int randomLocation_Y = rand() % 50 ;

            //위치 X : 0.0~ 0.99, 위치 Y : 0.5 ~ 0.99
            Particle2D tmp = Particle2D((float)randomLocation_X / 100.0, (float)randomLocation_Y / 100.0 + 0.5 , 0, 0, 0, -0.098);
            particles.push_back( tmp );
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
        for(int i=0; i<particles.size(); i++) {
            this->particles[i].Update_particle_Velocity(timestep);
        }
    }

    //입자들의 위치를 모두 update
    void Update_particles_Location() {
        for(int i=0; i<particles.size(); i++) {
            this->particles[i].Update_particle_Location(timestep);
        }
    }



};