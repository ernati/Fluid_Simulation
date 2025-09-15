#pragma once

#include <vector>
#include <cmath>
#include "../../../myVector/myVector2D.h"

// Metaball rendering constants
#define METABALL_THRESHOLD 1.0f
#define GRID_RESOLUTION 128
#define METABALL_RADIUS 0.1f
#define METABALL_STRENGTH 1.0f

// Structure to represent a triangle in the mesh
struct Triangle {
    Vector2D vertices[3];
    Vector2D normal;
    
    Triangle() {}
    Triangle(const Vector2D& v1, const Vector2D& v2, const Vector2D& v3) {
        vertices[0] = v1;
        vertices[1] = v2;
        vertices[2] = v3;
        calculateNormal();
    }
    
    void calculateNormal() {
        Vector2D edge1 = vertices[1] - vertices[0];
        Vector2D edge2 = vertices[2] - vertices[0];
        // For 2D, we'll use a simple perpendicular vector
        normal = Vector2D(-edge1.Y, edge1.X);
        normal = normal.normalize();
    }
};

// Structure to represent metaball mesh
struct MetaballMesh {
    std::vector<Triangle> triangles;
    std::vector<Vector2D> vertices;
    std::vector<unsigned int> indices;
    
    void clear() {
        triangles.clear();
        vertices.clear();
        indices.clear();
    }
    
    void addTriangle(const Vector2D& v1, const Vector2D& v2, const Vector2D& v3) {
        triangles.emplace_back(v1, v2, v3);
        
        // Add vertices for OpenGL rendering
        vertices.push_back(v1);
        vertices.push_back(v2);
        vertices.push_back(v3);
        
        // Add indices
        unsigned int baseIndex = static_cast<unsigned int>(indices.size());
        indices.push_back(baseIndex);
        indices.push_back(baseIndex + 1);
        indices.push_back(baseIndex + 2);
    }
};

// Grid cell for marching squares algorithm
struct GridCell {
    Vector2D position[4];  // Corner positions
    float value[4];        // Field values at corners
};

// Metaball class for fluid surface rendering
class MetaballRenderer {
private:
    int gridWidth, gridHeight;
    float cellSize;
    Vector2D gridMin, gridMax;
    std::vector<std::vector<float>> scalarField;
    MetaballMesh mesh;
    
    // Marching squares lookup tables
    static const int edgeTable[16];
    static const int triTable[16][5];
    
public:
    MetaballRenderer();
    MetaballRenderer(int width, int height, const Vector2D& minBounds, const Vector2D& maxBounds);
    
    // Initialize the metaball renderer
    void initialize(int width, int height, const Vector2D& minBounds, const Vector2D& maxBounds);
    
    // Generate scalar field from particle positions
    void generateScalarField(const std::vector<Vector2D>& particles);
    
    // Generate mesh using marching squares algorithm
    void generateMesh();
    
    // Get the generated mesh for rendering
    const MetaballMesh& getMesh() const { return mesh; }
    
    // Clear the current mesh and scalar field
    void clear();
    
    // Set metaball parameters
    void setThreshold(float threshold) { this->threshold = threshold; }
    void setRadius(float radius) { this->radius = radius; }
    void setStrength(float strength) { this->strength = strength; }
    
    // Get metaball parameters
    float getThreshold() const { return threshold; }
    float getRadius() const { return radius; }
    float getStrength() const { return strength; }
    
private:
    float threshold;
    float radius;
    float strength;
    
    // Calculate metaball field value at a point
    float calculateFieldValue(const Vector2D& point, const std::vector<Vector2D>& particles);
    
    // Interpolate between two points based on field values
    Vector2D interpolate(const Vector2D& p1, float val1, const Vector2D& p2, float val2);
    
    // Process a single grid cell for marching squares
    void processGridCell(int x, int y);
    
    // Get grid cell at specific coordinates
    GridCell getGridCell(int x, int y);
};

// Static lookup tables for marching squares algorithm
const int MetaballRenderer::edgeTable[16] = {
    0x0, 0x9, 0x3, 0xa, 0x6, 0xf, 0x5, 0xc,
    0xc, 0x5, 0xf, 0x6, 0xa, 0x3, 0x9, 0x0
};

const int MetaballRenderer::triTable[16][5] = {
    {-1, -1, -1, -1, -1},  // 0
    {0, 3, -1, -1, -1},    // 1
    {0, 1, -1, -1, -1},    // 2
    {1, 3, -1, -1, -1},    // 3
    {1, 2, -1, -1, -1},    // 4
    {0, 3, 1, 2, -1},      // 5
    {0, 2, -1, -1, -1},    // 6
    {2, 3, -1, -1, -1},    // 7
    {2, 3, -1, -1, -1},    // 8
    {0, 2, -1, -1, -1},    // 9
    {0, 3, 1, 2, -1},      // 10
    {1, 2, -1, -1, -1},    // 11
    {1, 3, -1, -1, -1},    // 12
    {0, 1, -1, -1, -1},    // 13
    {0, 3, -1, -1, -1},    // 14
    {-1, -1, -1, -1, -1}   // 15
};