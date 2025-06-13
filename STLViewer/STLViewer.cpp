// STLViewer.cpp : Defines the entry point for the application.
#include "STLViewer.h"
#include "STLLoader.h"
#include "MeshOperations.h"
#include "MeshRenderer.h"

std::string loadShaderFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << path << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
GLuint createShaderProgram(const std::string& vertPath, const std::string& fragPath) {
    std::string vertCode = loadShaderFromFile(vertPath);
    std::string fragCode = loadShaderFromFile(fragPath);

    const char* vShaderCode = vertCode.c_str();
    const char* fShaderCode = fragCode.c_str();

    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);

    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return program;
}

int main()
{
    //// Initialize GLFW and create window
    //glfwInit();
    //GLFWwindow* window = glfwCreateWindow(800, 600, "Mesh Renderer", NULL, NULL);
    //glfwMakeContextCurrent(window);

    //// Initialize GLAD
    //if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    //    std::cout << "Failed to initialize GLAD" << std::endl;
    //    return -1;
    //}
    //ASCII STL Mesh Loader (won't work on binary files)
    //std::shared_ptr<Mesh> mesh = STLLoader::load("..//Assets//Sphericon.stl");
    std::shared_ptr<Mesh> mesh = STLLoader::load("..//Assets//Sphericon.stl");
    //std::shared_ptr<Mesh> mesh = STLLoader::load("Square.stl");

    if (mesh->vertexCount() == 0) {
        std::cout << "No vertices loaded!" << std::endl;
    }
    else {
        std::cout << "Mesh loaded with " << mesh->vertexCount() << " vertices and "
            << mesh->triangleCount() << " triangles." << std::endl;
    }

    //Remove Duplicate Vertices
    MeshOperations::removeDuplicateVertices(*mesh);
    if (mesh->vertexCount() == 0) {
        std::cout << "No vertices loaded!" << std::endl;
    }
    else {
        std::cout << "Mesh after removing duplicate vertices has " << mesh->vertexCount() << " vertices." << std::endl;
    }

    ////Check if Triangle Face Normals Insterted correctly
    //const std::vector<Triangle>& triangles = mesh->getTriangles();
    //for (size_t i = 0; i < triangles.size(); ++i) {
    //    const glm::vec3& n = triangles[i].faceNormal;
    //    std::cout << "Triangle " << i << " normal: ("
    //        << n.x << ", " << n.y << ", " << n.z << ")\n";
    //}

    //Compute Per Vertex Normals
    MeshOperations::computePerVertexNormals(*mesh);
    const std::vector<Vertex>& tempVertices = mesh->getVertices();
    for (const auto& v : tempVertices) {
        std::cout << "Normal: (" << v.normal.x << ", " << v.normal.y << ", " << v.normal.z << ")\n";
    }

    MeshOperations::computeAdjacency(*mesh);
    MeshOperations::printNeighborCounts(*mesh);
    MeshOperations::printMeshDebugInfo(*mesh);

    std::vector<int> neighborCounts = MeshOperations::getNeighborCounts(*mesh);

    //// Load and compile shaders
    //GLuint shaderProgram = createShaderProgram("C://temp//mesh.vert.glsl", "C://temp//mesh.frag.glsl");

    //// Upload neighbor counts to MeshRenderer
    //MeshRenderer renderer;
    //renderer.setNeighborData(*mesh, neighborCounts);  // You'll implement this

    //// Enable depth testing
    //glEnable(GL_DEPTH_TEST);

    //// Main render loop
    //while (!glfwWindowShouldClose(window)) {
    //    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //    glUseProgram(shaderProgram);
    //    renderer.renderMesh(*mesh);

    //    glfwSwapBuffers(window);
    //    glfwPollEvents();
    //}

    //glfwTerminate();
    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.get();
    return 0;
}