#pragma once
#include <vector>
#include <glm.hpp>

struct Vertex {
    glm::vec3 position{ 0.0f, 0.0f, 0.0f }; 
    glm::vec3 normal{ 0.0f, 0.0f, 0.0f };
};

struct Triangle {
    int v1, v2, v3; // Vertex indices
    int adjacentTriangles[3] = { -1, -1, -1 }; // Optional adjacency info
    glm::vec3 faceNormal{ 0.0f, 0.0f, 0.0f };

	// Constructor for easy initialization
    Triangle(int a, int b, int c, glm::vec3 inNormal)
        : v1(a), v2(b), v3(c), faceNormal(inNormal) {
    }
    Triangle() = default; //Default Constructor

};

class Mesh {
public:
    // Basic operations
    void addVertex(const Vertex& vertex);
    void addTriangle(const Triangle& tri);

    // Access
    std::vector<Vertex>& getVertices() { return vertices; }
    std::vector<Triangle>& getTriangles() { return triangles; }

    // Basic info
    size_t vertexCount() const { return vertices.size(); }
    size_t triangleCount() const { return triangles.size(); }

    void clear();

private:
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;
};