// STLViewer.cpp : Defines the entry point for the application.
#include "STLViewer.h"
#include "STLLoader.h"
#include "MeshOperations.h"
#include "MeshRenderer.h"
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>


// Camera control state
float pitch = 0.0f;        // rotation around X
float yaw = 0.0f;          // rotation around Y
float distance = 3.0f;     // zoom / distance from mesh

bool rotating = false;
double lastX = 0.0, lastY = 0.0;

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

    int success;
    char infoLog[512];
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cerr << "Vertex Shader compilation failed:\n" << infoLog << std::endl;
    }

    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cerr << "Fragment Shader compilation failed:\n" << infoLog << std::endl;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "Shader Program linking failed:\n" << infoLog << std::endl;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return program;
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            rotating = true;
            glfwGetCursorPos(window, &lastX, &lastY);
        }
        else if (action == GLFW_RELEASE) {
            rotating = false;
        }
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    if (!rotating) return;

    float dx = static_cast<float>(xpos - lastX);
    float dy = static_cast<float>(ypos - lastY);
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.3f;
    yaw += dx * sensitivity;
    pitch += dy * sensitivity;

    // Clamp pitch
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    distance -= static_cast<float>(yoffset);
    if (distance < 1.0f) distance = 1.0f;
    if (distance > 20.0f) distance = 20.0f;
}


int main()
{
    // Initialize GLFW and create window
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(800, 600, "Mesh Renderer", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    //ASCII STL Mesh Loader (won't work on binary files)
    //std::shared_ptr<Mesh> mesh = STLLoader::load("..//Resources//Square.stl");
    std::shared_ptr<Mesh> mesh = STLLoader::load("..//Resources//Sphericon.stl");
    
    // Compute bounding box
    glm::vec3 min = mesh->getVertices()[0].position;
    glm::vec3 max = min;

    for (const auto& v : mesh->getVertices()) {
        min = glm::min(min, v.position);
        max = glm::max(max, v.position);
    }

    glm::vec3 center = (min + max) * 0.5f;
    float radius = glm::length(max - min) * 0.5f;

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

    //MeshOperations::computeAdjacency(*mesh);
    //MeshOperations::printNeighborCounts(*mesh);
    //MeshOperations::printMeshDebugInfo(*mesh);

    //std::vector<int> neighborCounts = MeshOperations::getNeighborCounts(*mesh);

    // Load and compile shaders
    GLuint shaderProgram = createShaderProgram("..//Shaders//mesh.vert.glsl", "..//Shaders//mesh.frag.glsl");

    // Upload neighbor counts to MeshRenderer
    MeshRenderer renderer;
    //renderer.setNeighborData(*mesh, neighborCounts);  // You'll implement this

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDisable(GL_CULL_FACE);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Main render loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);
        // Setup matrices
        glm::mat4 model = glm::translate(glm::mat4(1.0f), -center);
        model = glm::scale(model, glm::vec3(1.0f / radius)); // Normalize size

        // Convert spherical coordinates to cartesian camera position
        float radPitch = glm::radians(pitch);
        float radYaw = glm::radians(yaw);
        glm::vec3 cameraPos = glm::vec3(
            distance * cos(radPitch) * sin(glm::radians(yaw)),
            distance * sin(radPitch),
            distance * cos(radPitch) * cos(glm::radians(yaw))
        );

        glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

        glm::mat4 view = glm::lookAt(cameraPos, target, up);

        glm::mat4 projection = glm::perspective(glm::radians(45.0f),
            800.0f / 600.0f,
            0.1f, 100.0f);

        GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
        GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
        GLuint projLoc = glGetUniformLocation(shaderProgram, "projection");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        renderer.renderMesh(*mesh);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.get();
    return 0;
}