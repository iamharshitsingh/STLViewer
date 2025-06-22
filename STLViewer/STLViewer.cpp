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

int main() {
    // --- Initialize GLFW ---
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "Mesh Renderer", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // --- Initialize GLAD ---
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // --- Load Mesh ---
    //std::shared_ptr<Mesh> mesh = STLLoader::load("../Resources/Sphericon.stl");
    std::shared_ptr<Mesh> mesh = STLLoader::load("../Resources/Cube.stl");
    if (!mesh || mesh->vertexCount() == 0) {
        std::cerr << "Failed to load mesh or no vertices found!" << std::endl;
        return -1;
    }

    std::cout << "Loaded mesh with " << mesh->vertexCount() << " vertices and "
              << mesh->triangleCount() << " triangles." << std::endl;

    // --- Preprocess Mesh ---
    MeshOperations::removeDuplicateVertices(*mesh);
    std::cout << "After removing duplicates: " << mesh->vertexCount() << " vertices." << std::endl;

    MeshOperations::computePerVertexNormals(*mesh);
    MeshOperations::computeAdjacency(*mesh);
    MeshOperations::printNeighborCounts(*mesh);
    MeshOperations::printMeshDebugInfo(*mesh);

    // --- Compute Bounding Box & Normalize ---
    glm::vec3 min = mesh->getVertices()[0].position;
    glm::vec3 max = min;
    for (const auto& v : mesh->getVertices()) {
        min = glm::min(min, v.position);
        max = glm::max(max, v.position);
    }
    glm::vec3 center = (min + max) * 0.5f;
    float radius = glm::length(max - min) * 0.5f;

    // --- Load Shaders ---
    GLuint shaderProgram = createShaderProgram("../Shaders/mesh.vert.glsl",
                                               "../Shaders/mesh.frag.glsl");

    MeshRenderer renderer;

    // --- Main Render Loop ---
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shaderProgram);

        // --- Setup Matrices ---
        glm::mat4 model = glm::translate(glm::mat4(1.0f), -center);
        model = glm::scale(model, glm::vec3(1.0f / radius));

        float radPitch = glm::radians(pitch);
        float radYaw = glm::radians(yaw);
        glm::vec3 cameraPos = {
            distance * cos(radPitch) * sin(radYaw),
            distance * sin(radPitch),
            distance * cos(radPitch) * cos(radYaw)
        };

        glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(0.0f), glm::vec3(0, 1, 0));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        // --- Pass Uniforms ---
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // --- Render ---
        renderer.renderMesh(*mesh);
        renderer.renderNormals(*mesh);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // --- Cleanup ---
    glfwTerminate();
    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.get();
    return 0;
}
