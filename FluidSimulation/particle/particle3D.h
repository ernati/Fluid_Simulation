#pragma once

#include <cmath> //for sqrt

#include "myVector3D.h"

//위치, 속도를 가지고 있는 입자 클래스
class Particle3D {

public:

    Vector3D Location;
    Vector3D Velocity;
    Vector3D Acceleration;

    double radius;
    double mass;

    //==============================================

    Particle3D() {
        Location = Vector3D();
        Velocity = Vector3D();
        Acceleration = Vector3D();

        radius = 0.001;
        mass = 0.000001;
    }

    Particle3D(double Location_X, double Location_Y,double Location_Z, double Velocity_X, double Velocity_Y, double Velocity_Z,  double Acceleration_X, double Acceleration_Y, double Acceleration_Z) {
        Location = Vector3D(Location_X, Location_Y,Location_Z);
        Velocity = Vector3D(Velocity_X, Velocity_Y, Velocity_Z);
        Acceleration = Vector3D(Acceleration_X, Acceleration_Y, Acceleration_Z);

        radius = 0.001;
        mass = 0.000001;
    }

    ~Particle3D() {

    }

    // 어떤 한 점과의 거리 구하기
    double get_distance_from_point(Vector3D point) {
        double distance_pow = (Location.X - point.X) * (Location.X - point.X) + (Location.Y - point.Y) * (Location.Y - point.Y) + (Location.Z - point.Z) * (Location.Z - point.Z);
        return sqrt(distance_pow);
    }



    //=================등가속도======================================
    //입자 하나의 속도를 update
    void Update_particle_Velocity(double timestep) {
        this->Velocity = this->Velocity + this->Acceleration * timestep;
    }

    //입자 하나의 Location을 update   x = x + vt + 1/2*a*(t^2)
    void Update_particle_Location(double timestep) {
        this->Location = this->Location + this->Velocity * timestep + this->Acceleration * timestep * timestep * 0.5;

    }

};