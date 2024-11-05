#pragma once

#include "../particle/particle.h"
#include "../grid/grid.h"
#include <vector>

using namespace std;

class Particle_Simulator_2D {

public:
    //particle들을 담을 vector
    vector<Particle2D> particles;
    float timestep;
    MAC_Grid<Vector2D> velocity_grid;

    //기본 생성자
    Particle_Simulator_2D();

    //simulation 함수
    void particle_simulation();

};

Particle_Simulator_2D::Particle_Simulator_2D()
{
    ;
}

void Particle_Simulator_2D::particle_simulation()
{
    ;
}
