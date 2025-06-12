#pragma once
#include <glad/glad.h>
#include "Mesh.h" // Your mesh header

class MeshRenderer {
public:
    MeshRenderer();
    ~MeshRenderer();

    void renderMesh(Mesh& mesh);

private:
    unsigned int VAO, VBO, EBO;
    bool buffersCreated;

    void createBuffers();
    void deleteBuffers();
};