#include "STLLoader.h"

std::shared_ptr<Mesh> STLLoader::load(const std::string& filename) {
    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Can't open file!" << std::endl;
        return mesh;
    }

    std::string word;
    glm::vec3 currentNormal(0.0f);
    std::vector<int> currentTriIndices;

    while (file >> word) {
        if (word == "facet") {
            file >> word; 
            float nx, ny, nz;
            file >> nx >> ny >> nz;
            currentNormal = glm::vec3(nx, ny, nz);
        }
        else if (word == "vertex") {
            float x, y, z;
            file >> x >> y >> z;

            Vertex v;
            v.position = glm::vec3(x, y, z);
            v.normal = glm::vec3(0, 0, 0);

            mesh->addVertex(v);
            currentTriIndices.push_back(mesh->vertexCount() - 1);
        }
        else if (word == "endfacet") {
            // After reading 3 vertices, store the triangle
            if (currentTriIndices.size() == 3) {
                Triangle tri (currentTriIndices[0], currentTriIndices[0], currentTriIndices[0], currentNormal);
                mesh->addTriangle(tri);
            }
            currentTriIndices.clear();
        }
    }

    std::cout << "Loaded: " << mesh->vertexCount() << " vertices, "
        << mesh->triangleCount() << " triangles" << std::endl;

    return mesh;
}
