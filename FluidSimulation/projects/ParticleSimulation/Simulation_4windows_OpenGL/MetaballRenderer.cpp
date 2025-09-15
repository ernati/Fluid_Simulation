#include "MetaballRenderer.h"
#include <algorithm>
#include <cmath>

MetaballRenderer::MetaballRenderer() : 
    gridWidth(GRID_RESOLUTION), 
    gridHeight(GRID_RESOLUTION),
    cellSize(1.0f),
    threshold(METABALL_THRESHOLD),
    radius(METABALL_RADIUS),
    strength(METABALL_STRENGTH) {
    gridMin = Vector2D(-1.0f, -1.0f);
    gridMax = Vector2D(1.0f, 1.0f);
    initialize(gridWidth, gridHeight, gridMin, gridMax);
}

MetaballRenderer::MetaballRenderer(int width, int height, const Vector2D& minBounds, const Vector2D& maxBounds) :
    gridWidth(width),
    gridHeight(height),
    gridMin(minBounds),
    gridMax(maxBounds),
    threshold(METABALL_THRESHOLD),
    radius(METABALL_RADIUS),
    strength(METABALL_STRENGTH) {
    initialize(width, height, minBounds, maxBounds);
}

void MetaballRenderer::initialize(int width, int height, const Vector2D& minBounds, const Vector2D& maxBounds) {
    gridWidth = width;
    gridHeight = height;
    gridMin = minBounds;
    gridMax = maxBounds;
    
    cellSize = std::max((maxBounds.X - minBounds.X) / width, 
                       (maxBounds.Y - minBounds.Y) / height);
    
    // Initialize scalar field grid
    scalarField.resize(gridHeight);
    for (int i = 0; i < gridHeight; i++) {
        scalarField[i].resize(gridWidth, 0.0f);
    }
}

void MetaballRenderer::generateScalarField(const std::vector<Vector2D>& particles) {
    // Clear previous field values
    for (int y = 0; y < gridHeight; y++) {
        for (int x = 0; x < gridWidth; x++) {
            scalarField[y][x] = 0.0f;
        }
    }
    
    // Calculate field values for each grid point
    for (int y = 0; y < gridHeight; y++) {
        for (int x = 0; x < gridWidth; x++) {
            Vector2D gridPoint;
            gridPoint.X = gridMin.X + (x * (gridMax.X - gridMin.X)) / (gridWidth - 1);
            gridPoint.Y = gridMin.Y + (y * (gridMax.Y - gridMin.Y)) / (gridHeight - 1);
            
            scalarField[y][x] = calculateFieldValue(gridPoint, particles);
        }
    }
}

float MetaballRenderer::calculateFieldValue(const Vector2D& point, const std::vector<Vector2D>& particles) {
    float totalValue = 0.0f;
    
    for (const auto& particle : particles) {
        Vector2D diff = point - particle;
        float distance = sqrt(diff.X * diff.X + diff.Y * diff.Y);
        
        if (distance < radius && distance > 0.0f) {
            // Use inverse square falloff for metaball field
            float influence = strength * radius * radius / (distance * distance);
            totalValue += influence;
        }
    }
    
    return totalValue;
}

void MetaballRenderer::generateMesh() {
    mesh.clear();
    
    // Process each grid cell using marching squares
    for (int y = 0; y < gridHeight - 1; y++) {
        for (int x = 0; x < gridWidth - 1; x++) {
            processGridCell(x, y);
        }
    }
}

void MetaballRenderer::processGridCell(int x, int y) {
    GridCell cell = getGridCell(x, y);
    
    // Determine configuration index
    int configIndex = 0;
    if (cell.value[0] > threshold) configIndex |= 1;
    if (cell.value[1] > threshold) configIndex |= 2;
    if (cell.value[2] > threshold) configIndex |= 4;
    if (cell.value[3] > threshold) configIndex |= 8;
    
    // Skip if no surface intersection
    if (configIndex == 0 || configIndex == 15) return;
    
    // Calculate edge intersections
    Vector2D edgePoints[4];
    bool hasEdge[4] = {false, false, false, false};
    
    // Bottom edge (0-1)
    if ((configIndex & 1) != (configIndex & 2)) {
        edgePoints[0] = interpolate(cell.position[0], cell.value[0], 
                                   cell.position[1], cell.value[1]);
        hasEdge[0] = true;
    }
    
    // Right edge (1-2)
    if ((configIndex & 2) != (configIndex & 4)) {
        edgePoints[1] = interpolate(cell.position[1], cell.value[1], 
                                   cell.position[2], cell.value[2]);
        hasEdge[1] = true;
    }
    
    // Top edge (2-3)
    if ((configIndex & 4) != (configIndex & 8)) {
        edgePoints[2] = interpolate(cell.position[2], cell.value[2], 
                                   cell.position[3], cell.value[3]);
        hasEdge[2] = true;
    }
    
    // Left edge (3-0)
    if ((configIndex & 8) != (configIndex & 1)) {
        edgePoints[3] = interpolate(cell.position[3], cell.value[3], 
                                   cell.position[0], cell.value[0]);
        hasEdge[3] = true;
    }
    
    // Generate triangles based on configuration
    const int* triangleConfig = triTable[configIndex];
    
    for (int i = 0; i < 4; i += 2) {
        if (triangleConfig[i] == -1) break;
        
        if (i + 1 < 4 && triangleConfig[i + 1] != -1) {
            int edge1 = triangleConfig[i];
            int edge2 = triangleConfig[i + 1];
            
            if (hasEdge[edge1] && hasEdge[edge2]) {
                // For 2D, we create line segments instead of triangles
                // But to maintain triangle structure for rendering, we create degenerate triangles
                Vector2D midPoint = (edgePoints[edge1] + edgePoints[edge2]) * 0.5f;
                mesh.addTriangle(edgePoints[edge1], edgePoints[edge2], midPoint);
            }
        }
    }
}

GridCell MetaballRenderer::getGridCell(int x, int y) {
    GridCell cell;
    
    // Calculate corner positions
    float stepX = (gridMax.X - gridMin.X) / (gridWidth - 1);
    float stepY = (gridMax.Y - gridMin.Y) / (gridHeight - 1);
    
    cell.position[0] = Vector2D(gridMin.X + x * stepX, gridMin.Y + y * stepY);       // Bottom-left
    cell.position[1] = Vector2D(gridMin.X + (x+1) * stepX, gridMin.Y + y * stepY);   // Bottom-right
    cell.position[2] = Vector2D(gridMin.X + (x+1) * stepX, gridMin.Y + (y+1) * stepY); // Top-right
    cell.position[3] = Vector2D(gridMin.X + x * stepX, gridMin.Y + (y+1) * stepY);   // Top-left
    
    // Get scalar field values
    cell.value[0] = scalarField[y][x];
    cell.value[1] = scalarField[y][x+1];
    cell.value[2] = scalarField[y+1][x+1];
    cell.value[3] = scalarField[y+1][x];
    
    return cell;
}

Vector2D MetaballRenderer::interpolate(const Vector2D& p1, float val1, const Vector2D& p2, float val2) {
    if (std::abs(val1 - val2) < 1e-6f) {
        return (p1 + p2) * 0.5f;
    }
    
    float t = (threshold - val1) / (val2 - val1);
    t = std::max(0.0f, std::min(1.0f, t)); // Clamp to [0, 1]
    
    return p1 + (p2 - p1) * t;
}

void MetaballRenderer::clear() {
    mesh.clear();
    
    for (int y = 0; y < gridHeight; y++) {
        for (int x = 0; x < gridWidth; x++) {
            scalarField[y][x] = 0.0f;
        }
    }
}