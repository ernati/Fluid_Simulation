#pragma once

#include <cmath> //for sqrt

#include "myVector2D.h"

//위치, 속도를 가지고 있는 입자 클래스
class Particle2D {

public:

    Vector2D Location;
    Vector2D Velocity;
    Vector2D Acceleration;

    //==============================================

    Particle2D() {
        Location = Vector2D();
        Velocity = Vector2D();
        Acceleration = Vector2D();
    }

    Particle2D(float Location_X, float Location_Y, float Velocity_X, float Velocity_Y, float Acceleration_X, float Acceleration_Y) {
        Location = Vector2D(Location_X, Location_Y);
        Velocity = Vector2D(Velocity_X, Velocity_Y);
        Acceleration = Vector2D(Acceleration_X, Acceleration_Y);
    }

    ~Particle2D() {

    }

    // 어떤 한 점과의 거리 구하기
    float get_distance_from_point(Vector2D point) {
        float distance_pow = (Location.X - point.X) * (Location.X - point.X) + (Location.Y - point.Y) * (Location.Y - point.Y);
        return sqrt(distance_pow);
    }



    //=================등가속도======================================
    //입자 하나의 속도를 update
    void Update_particle_Velocity(float timestep) {
        this->Velocity = this->Velocity + this->Acceleration * timestep;
    }

    //입자 하나의 Location을 update   x = x + vt + 1/2*a*(t^2)
    void Update_particle_Location(float timestep) {
        this->Location = this->Location + this->Velocity * timestep + this->Acceleration * timestep * timestep * 0.5;

        if (this->Location.Y < 0) {
            this->Location.Y = 0.51;
            this->Velocity.X = 0.0; this->Velocity.Y = 0.0;
        }
    }

};