#include "Mesh.h"

void Mesh::addVertex(const Vertex& vertex) {
    vertices.push_back(vertex);
}

void Mesh::addTriangle(int v1, int v2, int v3) {
    triangles.push_back({ v1, v2, v3 });
}

void Mesh::clear() {
    vertices.clear();
    triangles.clear();
}