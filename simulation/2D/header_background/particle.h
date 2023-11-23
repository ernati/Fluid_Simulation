#pragma once

#include <cmath> //for sqrt

#include "myVector2D.h"

//위치, 속도, 가속도, 반지름, 질량을 가지고 있는 입자 클래스
class Particle2D {

public:

    Vector2D Location;
    Vector2D Velocity;
    Vector2D Acceleration;

    double radius;
    double mass;

    //==============================================

    Particle2D() {
        Location = Vector2D();
        Velocity = Vector2D();
        Acceleration = Vector2D();

        radius = 0.05;
        mass = 0.0025;
    }

    Particle2D( double Location_X, double Location_Y, double Velocity_X, double Velocity_Y, double Acceleration_X, double Acceleration_Y ) {
        Location = Vector2D(Location_X, Location_Y);
        Velocity = Vector2D(Velocity_X, Velocity_Y);
        Acceleration = Vector2D( Acceleration_X, Acceleration_Y );

        radius = 0.05;
        mass = 0.0025;
    }

    ~Particle2D() {

    }

    // 어떤 한 점과의 거리 구하기
    double get_distance_from_point(Vector2D point) {
        double distance_pow = (Location.X - point.X) * (Location.X - point.X) + (Location.Y - point.Y) * (Location.Y - point.Y);
        return sqrt(distance_pow);
    }



    //=================등가속도======================================
    //객체지향 원리에 따르면, 이 함수들은 등가속도 운동 시뮬레이터 함수 안에 있는게 맞을 것 같다.
    //다만, 이미 구현해놓은 것이 있으므로 그대로 둔다.
    //입자 하나의 속도를 update - euler
    void Update_particle_Velocity(double timestep) {
        this->Velocity = this->Velocity + this->Acceleration * timestep;
    }

    //입자 하나의 Location을 update   x = x + vt + 1/2*a*(t^2)
    void Update_particle_Location(double timestep) {
        this->Location = this->Location + this->Velocity * timestep + this->Acceleration * timestep * timestep * 0.5;

    }

};
