#include "MeshOperations.h"

void MeshOperations::removeDuplicateVertices(Mesh& inMesh) {
    const std::vector<Vertex>& oldVertices = inMesh.getVertices();
    std::vector<Vertex> newVertices;
    std::unordered_map<glm::vec3, int, Vec3Hash, Vec3Equal> positionToIndex;
    std::vector<int> remap(oldVertices.size(), -1);

    //Build new list of unique vertices
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

    //Remap triangle indices
    for (auto& tri : inMesh.getTriangles()) {
        tri.v1 = remap[tri.v1];
        tri.v2 = remap[tri.v2];
        tri.v3 = remap[tri.v3];
    }

    //Replace vertex list
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
            v.normal = glm::vec3(0.0f); 
        }
    }
}

void MeshOperations::computeAdjacency(Mesh& inMesh) {
    auto& triangles = inMesh.getTriangles();
    std::unordered_map<Edge, std::vector<int>, EdgeHash> edgeToFaces;

    //Build edge-to-faces map
    for (int i = 0; i < static_cast<int>(triangles.size()); ++i) {
        const Triangle& tri = triangles[i];
        edgeToFaces[Edge(tri.v1, tri.v2)].push_back(i);
        edgeToFaces[Edge(tri.v2, tri.v3)].push_back(i);
        edgeToFaces[Edge(tri.v3, tri.v1)].push_back(i);
    }

    //Assign adjacent triangle indices
    for (int i = 0; i < static_cast<int>(triangles.size()); ++i) {
        Triangle& tri = triangles[i];
        std::array<Edge, 3> edges = {
            Edge(tri.v1, tri.v2),
            Edge(tri.v2, tri.v3),
            Edge(tri.v3, tri.v1)
        };

        for (int e = 0; e < 3; ++e) {
            const auto& faceList = edgeToFaces[edges[e]];

            for (int neighbor : faceList) {
                if (neighbor != i) {
                    tri.adjacentTriangles[e] = neighbor;
                    break; //Only one adjacent triangle per edge
                }
            }
        }
    }
}

void MeshOperations::printNeighborCounts(const Mesh& inMesh) {
    const auto& triangles = inMesh.getTriangles();

    for (size_t i = 0; i < triangles.size(); ++i) {
        int count = 0;
        for (int j = 0; j < 3; ++j) {
            if (triangles[i].adjacentTriangles[j] != -1) {
                ++count;
            }
        }
        std::cout << "Triangle " << i << " has " << count << " neighbor(s).\n";
    }
}