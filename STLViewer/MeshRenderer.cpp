#include "MeshRenderer.h"
#include <iostream>

MeshRenderer::MeshRenderer() {
    buffersCreated = false;
    VAO = VBO = EBO = 0;
    createBuffers();
}

MeshRenderer::~MeshRenderer() {
    deleteBuffers();
}

void MeshRenderer::createBuffers() {
    // Create the OpenGL buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    buffersCreated = true;
}

void MeshRenderer::deleteBuffers() {
    if (buffersCreated) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        buffersCreated = false;
    }
}

void MeshRenderer::renderMesh(Mesh& mesh) {
    // Get mesh data
    std::vector<Vertex>& vertices = mesh.getVertices();
    std::vector<Triangle>& triangles = mesh.getTriangles();

    if (vertices.empty() || triangles.empty()) {
        return; // Nothing to render
    }

    // Bind vertex array
    glBindVertexArray(VAO);

    // Upload vertex data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    // Set up vertex attributes
    // Position attribute (location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal attribute (location 1) 
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Create index array from triangles
    std::vector<unsigned int> indices;
    for (int i = 0; i < triangles.size(); i++) {
        indices.push_back(triangles[i].v1);
        indices.push_back(triangles[i].v2);
        indices.push_back(triangles[i].v3);
    }

    // Upload indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // Draw the mesh
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    // Unbind
    glBindVertexArray(0);
}