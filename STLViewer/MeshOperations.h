#pragma once

#include <unordered_map>
#include <vector>
#include <cmath>
#include "Mesh.h"

class MeshOperations {
public:
    // Removes duplicate vertices in the mesh and updates triangle indices
    static void removeDuplicateVertices(Mesh& inMesh);
    static void computePerVertexNormals(Mesh& inMesh);
private:
    // Helper to hash glm::vec3 (position)
    struct Vec3Hash {
        size_t operator()(const glm::vec3& v) const {
            return std::hash<float>()(v.x) ^ std::hash<float>()(v.y) ^ std::hash<float>()(v.z);
        }
    };

    // Helper to compare glm::vec3 with tolerance
    struct Vec3Equal {
        bool operator()(const glm::vec3& a, const glm::vec3& b) const {
            const float epsilon = 1e-6f;
            return std::abs(a.x - b.x) < epsilon &&
                std::abs(a.y - b.y) < epsilon &&
                std::abs(a.z - b.z) < epsilon;
        }
    };

};
