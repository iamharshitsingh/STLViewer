// STLViewer.cpp : Defines the entry point for the application.
#include "STLViewer.h"
#include "STLLoader.h"
#include "MeshOperations.h"
#include "MeshRenderer.h"
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
        // Initialize GLFW and create window
        glfwInit();
        GLFWwindow* window = glfwCreateWindow(800, 600, "Mesh Renderer", NULL, NULL);
        glfwMakeContextCurrent(window);

        // Initialize GLAD
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return -1;
        }

        //ASCII STL Mesh Loader (won't work on binary files)
		std::shared_ptr<Mesh> mesh = STLLoader::load("C://temp//Square.stl");
		//std::shared_ptr<Mesh> mesh = STLLoader::load("C://temp//Sphericon.stl");
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

        // Main render loop
        MeshRenderer renderer;
        while (!glfwWindowShouldClose(window)) {
            // Clear screen
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Render your mesh
            renderer.renderMesh(*mesh);

            // Swap buffers and poll events
            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        glfwTerminate();

    }
	std::cout << "Press Enter to exit..." << std::endl;
    std::cin.get();
    return 0;
}
