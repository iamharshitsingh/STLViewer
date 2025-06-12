// STLViewer.cpp : Defines the entry point for the application.
#include "STLViewer.h"
#include "STLLoader.h"
#include "MeshOperations.h"

int main()
{
	if (false) //Temporay switch to toggle between loading STL and initializing OpenGL context
    {
        if (!glfwInit()) return -1;

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        GLFWwindow* window = glfwCreateWindow(800, 600, "STL Viewer", nullptr, nullptr);
        if (!window) { glfwTerminate(); return -1; }

        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "Failed to initialize GLAD\n";
            return -1;
        }

        while (!glfwWindowShouldClose(window)) {
            glClear(GL_COLOR_BUFFER_BIT);
            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        glfwDestroyWindow(window);
        glfwTerminate();
    }
    else
    {
		std::shared_ptr<Mesh> mesh = STLLoader::load("C://temp//Square.stl");
		if (mesh->vertexCount() == 0) {
			std::cout << "No vertices loaded!" << std::endl;
		}
		else {
			std::cout << "Mesh loaded with " << mesh->vertexCount() << " vertices and "
				<< mesh->triangleCount() << " triangles." << std::endl;
		}

        MeshOperations::removeDuplicateVertices(*mesh);
        
        if (mesh->vertexCount() == 0) {
            std::cout << "No vertices loaded!" << std::endl;
        }
        else {
            std::cout << "Mesh after removing duplicate vertices has " << mesh->vertexCount() << " vertices." << std::endl;
        }
    }
    std::cin.get();
    return 0;
}
