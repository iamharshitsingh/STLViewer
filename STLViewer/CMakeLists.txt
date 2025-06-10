# CMakeList.txt : CMake project for STLViewer, include source and define
# project specific logic here.
#

# Collect source files
set(SOURCES
    ${PROJECT_SOURCE_DIR}/STLViewer/STLViewer.cpp
    ${PROJECT_SOURCE_DIR}/STLViewer/STLViewer.h
    ${PROJECT_SOURCE_DIR}/ThirdPartyLibraries/GLAD/src/glad.c
)

# Create executable from sources
add_executable(STLViewer ${SOURCES} "STLLoader.cpp" "STLLoader.h" "Mesh.h" "Mesh.cpp")

# C++ Standard
set_property(TARGET STLViewer PROPERTY CXX_STANDARD 20)

# Include directories
target_include_directories(STLViewer PRIVATE
    ${PROJECT_SOURCE_DIR}/STLViewer
    ${PROJECT_SOURCE_DIR}/ThirdPartyLibraries/GLAD/include
    ${PROJECT_SOURCE_DIR}/ThirdPartyLibraries/GLFW/include
    ${PROJECT_SOURCE_DIR}/ThirdPartyLibraries/GLM
)

# Link directories (Windows only)
if (MSVC)
    target_link_directories(STLViewer PRIVATE
        ${PROJECT_SOURCE_DIR}/ThirdPartyLibraries/GLFW/lib
    )
endif()

# Link libraries
if (MSVC)
    target_link_libraries(STLViewer glfw3 opengl32)
else()
    target_link_libraries(STLViewer glfw GL dl)
endif()