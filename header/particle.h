#pragma once

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

    Particle2D( float Location_X, float Location_Y, float Velocity_X, float Velocity_Y, float Acceleration_X, float Acceleration_Y ) {
        Location = Vector2D(Location_X, Location_Y);
        Velocity = Vector2D(Velocity_X, Velocity_Y);
        Acceleration = Vector2D( Acceleration_X, Acceleration_Y );
    }

    ~Particle2D() {

    }

    //입자 하나의 속도를 update
    void Update_particle_Velocity(float timestep) {
        this->Velocity = this->Velocity + this->Velocity * timestep;
    }

    //입자 하나의 Location을 update
    void Update_particle_Location(float timestep) {
        this->Location = this->Location + this->Velocity * timestep;
    }

};