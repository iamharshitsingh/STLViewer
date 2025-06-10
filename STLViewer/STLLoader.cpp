#include "STLLoader.h"

std::shared_ptr<Mesh> STLLoader::load(const std::string& filename) {
    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Can't open file!" << std::endl;
        return mesh;
    }

    std::string word;

    while (file >> word) {
        if (word == "vertex") {
            float x, y, z;
            file >> x >> y >> z;

            Vertex v;
            v.position = glm::vec3(x, y, z);
            v.normal = glm::vec3(0, 0, 0);

            mesh->addVertex(v);

            // Every 3 vertices make a triangle
            if (mesh->vertexCount() % 3 == 0) {
                int last = (int)mesh->vertexCount() - 1;
                mesh->addTriangle(last - 2, last - 1, last);
            }
        }
    }

    std::cout << "Loaded: " << mesh->vertexCount() << " vertices, "
        << mesh->triangleCount() << " triangles" << std::endl;

    return mesh;
}