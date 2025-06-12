#include "Mesh.h"

void Mesh::addVertex(const Vertex& vertex) {
    vertices.emplace_back(vertex);
}

void Mesh::addTriangle(const Triangle& tri) {
    triangles.push_back(tri);
}

void Mesh::clear() {
    vertices.clear();
    triangles.clear();
}