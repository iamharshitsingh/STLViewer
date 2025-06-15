#include "MeshRenderer.h"
#include <vector>
#include <iostream>
#include "MeshOperations.h"

MeshRenderer::MeshRenderer()
    : VAO(0), VBO(0), EBO(0), buffersCreated(false) {
}

MeshRenderer::~MeshRenderer() {
    deleteBuffers();
}

void MeshRenderer::createBuffers() {
    if (buffersCreated) return;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    buffersCreated = true;
}

void MeshRenderer::deleteBuffers() {
    if (!buffersCreated) return;

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    buffersCreated = false;
}

void MeshRenderer::renderMesh(Mesh& mesh) {
    if (mesh.triangleCount() == 0 || mesh.vertexCount() == 0)
        return;

    createBuffers();

    // Compute adjacency if not already done
    MeshOperations::computeAdjacency(mesh);
    std::vector<int> neighborCounts = MeshOperations::getNeighborCounts(mesh);

    // Generate colored vertex data
    struct VertexData {
        glm::vec3 position;
        glm::vec3 color;
    };

    std::vector<VertexData> vertexData;
    std::vector<unsigned int> indices;

    const auto& triangles = mesh.getTriangles();
    const auto& vertices = mesh.getVertices();

    for (size_t i = 0; i < triangles.size(); ++i) {
        const Triangle& tri = triangles[i];
        glm::vec3 color;

        switch (neighborCounts[i]) {
        case 0: color = glm::vec3(1.0f, 0.0f, 0.0f); break; // red
        case 1: color = glm::vec3(1.0f, 1.0f, 0.0f); break; // yellow
        case 2: color = glm::vec3(0.0f, 1.0f, 0.0f); break; // green
        case 3: color = glm::vec3(0.0f, 0.0f, 1.0f); break; // blue
        default: color = glm::vec3(1.0f, 1.0f, 1.0f); break; // white
        }

        unsigned int baseIndex = static_cast<unsigned int>(vertexData.size());

        vertexData.push_back({ vertices[tri.v1].position, color });
        vertexData.push_back({ vertices[tri.v2].position, color });
        vertexData.push_back({ vertices[tri.v3].position, color });

        indices.push_back(baseIndex);
        indices.push_back(baseIndex + 1);
        indices.push_back(baseIndex + 2);
    }

    // Upload data to GPU
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(VertexData), vertexData.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
    glEnableVertexAttribArray(0);

    // Color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, color));
    glEnableVertexAttribArray(1);

    // Draw
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    setNeighborData(mesh, neighborCounts);
}

void MeshRenderer::setNeighborData(const Mesh& mesh, const std::vector<int>& neighborCounts) {
    if (!buffersCreated)
        createBuffers();

    // Expand triangle-based neighbor data to vertex-level (duplicated per triangle)
    std::vector<float> expandedNeighborData;
    const auto& triangles = mesh.getTriangles();

    for (size_t i = 0; i < triangles.size(); ++i) {
        float value = static_cast<float>(neighborCounts[i]);
        // Push one value per triangle vertex (3 vertices)
        expandedNeighborData.push_back(value);
        expandedNeighborData.push_back(value);
        expandedNeighborData.push_back(value);
    }

    glGenBuffers(1, &neighborVBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, neighborVBO);
    glBufferData(GL_ARRAY_BUFFER, expandedNeighborData.size() * sizeof(float), expandedNeighborData.data(), GL_STATIC_DRAW);

    // Set attribute pointer location 2 (match vertex shader)
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}
