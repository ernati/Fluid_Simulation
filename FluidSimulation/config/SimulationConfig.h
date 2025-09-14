#pragma once

// Simulation Configuration Constants
// This file contains all configurable parameters for the fluid simulation

namespace SimulationConfig {
    
    // Particle Configuration
    const int DEFAULT_PARTICLE_COUNT = 8000;      // Default number of particles
    const double DEFAULT_PARTICLE_RADIUS = 0.05;  // Default particle radius
    
    // Grid Configuration  
    const int DEFAULT_GRID_SIZE = 40;              // Default grid dimensions (NxN)
    
    // Time Configuration
    const double DEFAULT_TIMESTEP = 0.06;         // Default simulation timestep
    const int IDLE_UPDATE_INTERVAL = 120;         // Milliseconds between updates
    
    // Physical Properties
    const double DEFAULT_DENSITY = 1.0;           // Default fluid density
    const double DEFAULT_GRAVITY = -9.8;          // Default gravity acceleration
    
    // Boundary Configuration
    const double SIMULATION_BOUNDARY_MIN = 0.0;   // Minimum simulation boundary
    const double SIMULATION_BOUNDARY_MAX = 1.0;   // Maximum simulation boundary
    
    // Random Particle Generation
    const int PARTICLE_RANDOM_RANGE = 200;        // Range for random particle positioning
    const int PARTICLE_POSITION_OFFSET_X = 100;   // X offset for particle positioning  
    const int PARTICLE_POSITION_OFFSET_Y = 600;   // Y offset for particle positioning
    const double PARTICLE_POSITION_SCALE = 1000.0; // Scale factor for particle positioning
    const double PARTICLE_VELOCITY_SCALE = 500.0;  // Scale factor for particle velocity
    const double PARTICLE_MASS_SCALE = 1000.0;     // Scale factor for particle mass
    
    // Window Configuration
    const int DEFAULT_WINDOW_WIDTH = 1024;        // Default window width
    const int DEFAULT_WINDOW_HEIGHT = 1024;       // Default window height
    
    // Menu Options
    const int DEFAULT_SIMULATION_OPTION = 3;      // Default simulation mode
    
}