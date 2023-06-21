#pragma once

#include <myVector2D.h>
#include <particle.h>
#include <vector>

//난수 생성
#include<cstdlib> //rand(), srand()
#include<ctime>

using namespace std;

    //등가속도 운동 시뮬레이션 - v_n+1_ = v_n_ + a*t
class Constant_Acceleration_Simulator {

private :
    //particle들을 담을 vector
    vector<Particle2D> particles;
    int timestep;

public :
    //timestep의 default = 60
    timestep = 60;

    //아무 입력값이 없다면 기본 particle 수는 100개
    Constant_Acceleration_Simulator() {
        srand((unsigned int)time(NULL));
        for(int i = 0; i<100; i++) {
            //0~99 난수 생성    
            int randomLocation_X = rand() % 100 ;
            //0~49 난수 생성
            int randomLocation_Y = rand() % 50 ;

            //위치 X : 0.0~ 0.99, 위치 Y : 0.5 ~ 0.99
            Particle2D tmp = Particle2D( float ( randomLocation_X / 100 ), float ( randomLocation_Y / 100 )+ 0.5,0,0,0,-9.8 );
            particles.push_back( tmp );
        }
    }

    Constant_Acceleration_Simulator( int particle_number ) {
        srand((unsigned int)time(NULL));
        for(int i = 0; i<particle_number; i++) {
            //0~99 난수 생성    
            int randomLocation_X = rand() % 100 ;
            //0~49 난수 생성
            int randomLocation_Y = rand() % 50 ;

            //위치 X : 0.0~ 0.99, 위치 Y : 0.5 ~ 0.99
            Particle2D tmp = Particle2D( float ( randomLocation_X / 100 ), float ( randomLocation_Y / 100 )+ 0.5,0,0,0,-9.8 );
            particles.push_back( tmp );
        }
    }

    //입자 하나의 속도를 update
    void Update_particle_Velocity( Particle2D& particle ) {
        particle.Velocity = particle.Velocity + particle.Velocity * timestep ;
    }

    void Update_particle_Location( Particle2D& particle ) {
        particle.Location = particle.Location + particle.Velocity * timestep ;
    }

    //입자들의 속도를 모두 update
    void Update_particles_Velocity() {
        for(int i=0; i<particles.size; i++) {
            Update_particle_Velocity( particles[i] );
        }
    }

    //입자들의 위치를 모두 update
    void Update_particles_Location() {
        for(int i=0; i<particles.size; i++) {
            Update_particle_Location( particles[i] );
        }
    }
};