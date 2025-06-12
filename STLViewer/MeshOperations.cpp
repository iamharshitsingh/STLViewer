#include "MeshOperations.h"

void MeshOperations::removeDuplicateVertices(Mesh& inMesh) {
    const std::vector<Vertex>& oldVertices = inMesh.getVertices();
    std::vector<Vertex> newVertices;
    std::unordered_map<glm::vec3, int, Vec3Hash, Vec3Equal> positionToIndex;
    std::vector<int> remap(oldVertices.size(), -1);

    // Build new list of unique vertices
    for (size_t i = 0; i < oldVertices.size(); ++i) {
        const glm::vec3& pos = oldVertices[i].position;

        auto it = positionToIndex.find(pos);
        if (it == positionToIndex.end()) {
            int newIndex = static_cast<int>(newVertices.size());
            newVertices.push_back(oldVertices[i]);
            positionToIndex[pos] = newIndex;
            remap[i] = newIndex;
        }
        else {
            remap[i] = it->second;
        }
    }

    // Remap triangle indices
    for (auto& tri : inMesh.getTriangles()) {
        tri.v1 = remap[tri.v1];
        tri.v2 = remap[tri.v2];
        tri.v3 = remap[tri.v3];
    }

    // Replace vertex list
    inMesh.getVertices() = std::move(newVertices);
}

void MeshOperations::computePerVertexNormals(Mesh& inMesh) {
    std::vector<Vertex>& vertices = inMesh.getVertices();
    const std::vector<Triangle>& triangles = inMesh.getTriangles();

    //Resetting to 0.0f
    for (auto& v : vertices) {
        v.normal = glm::vec3(0.0f);
    }

    //Accumulate triangle normals
    for (const auto& tri : triangles) {
        const glm::vec3& tempfaceNormal = tri.faceNormal;

        vertices[tri.v1].normal += tempfaceNormal;
        vertices[tri.v2].normal += tempfaceNormal;
        vertices[tri.v3].normal += tempfaceNormal;
    }

    //Normalize
    for (auto& v : vertices) {
        if (glm::length(v.normal) > 1e-6f) {
            v.normal = glm::normalize(v.normal);
        }
        else {
            v.normal = glm::vec3(0.0f); // or fallback to something safe
        }
    }
}
