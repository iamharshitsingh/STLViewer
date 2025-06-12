#include "MeshOperations.h"

void MeshOperations::removeDuplicateVertices(Mesh& mesh) {
    const std::vector<Vertex>& oldVertices = mesh.getVertices();
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
    for (auto& tri : mesh.getTriangles()) {
        tri.v1 = remap[tri.v1];
        tri.v2 = remap[tri.v2];
        tri.v3 = remap[tri.v3];
    }

    // Replace vertex list
    mesh.getVertices() = std::move(newVertices);
}
