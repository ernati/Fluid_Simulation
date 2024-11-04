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
    public MAC_Grid<Vector2D> velocity_grid;

    //기본 생성자
    Particle_Simulator_2D();

    //simulation 함수
    virtual void particle_simulation();

};

