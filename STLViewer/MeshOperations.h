#pragma once
#include <unordered_map>
#include <vector>
#include <cmath>
#include <array>
#include <iostream>
#include "Mesh.h"


class MeshOperations {
public:
    //Removes duplicate vertices in the mesh and updates triangle indices
    static void removeDuplicateVertices         (Mesh& inMesh);
    static void computePerVertexNormals         (Mesh& inMesh);
    static void computeAdjacency                (Mesh& inMesh);
    static void printNeighborCounts             (const Mesh& inMesh);
    static std::vector<int> getNeighborCounts   (const Mesh& inMesh);
    static void printMeshDebugInfo              (const Mesh& inMesh);
private:
    //Helper to hash glm::vec3 (position)
    struct Vec3Hash {
        size_t operator()(const glm::vec3& v) const {
            return std::hash<float>()(v.x) ^ std::hash<float>()(v.y) ^ std::hash<float>()(v.z);
        }
    };

    //Helper to compare glm::vec3 with tolerance
    struct Vec3Equal {
        bool operator()(const glm::vec3& a, const glm::vec3& b) const {
            const float epsilon = 1e-6f;
            return std::abs(a.x - b.x) < epsilon &&
                std::abs(a.y - b.y) < epsilon &&
                std::abs(a.z - b.z) < epsilon;
        }
    };

    //Edge struct used for hashing
    struct Edge {
        int a, b;

        Edge(int u, int v) {
            a = std::min(u, v);
            b = std::max(u, v);
        }

        bool operator==(const Edge& other) const {
            return a == other.a && b == other.b;
        }
    };

    //Custom hash function for Edge
    struct EdgeHash {
        std::size_t operator()(const Edge& e) const {
            return std::hash<int>()(e.a) ^ (std::hash<int>()(e.b) << 1);
        }
    };
};
