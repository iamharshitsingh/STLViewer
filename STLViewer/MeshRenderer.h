#pragma once
#include <glad/glad.h>
#include "Mesh.h" // Your mesh header

class MeshRenderer {
public:
    MeshRenderer();
    ~MeshRenderer();

    void renderMesh(Mesh& mesh);
    void setNeighborData(const Mesh& mesh, const std::vector<int>& neighborCounts);

private:
    unsigned int VAO, VBO, EBO;
    bool buffersCreated;
    unsigned int neighborVBO = 0;

    void createBuffers();
    void deleteBuffers();
};