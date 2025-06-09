# STLViewer

**STLViewer** is a minimal C++ OpenGL application that:

- Loads an ASCII STL file
- Removes duplicate vertices
- Colors each face based on number of connected neighbors
- Computes and displays per-vertex normals
- Uses modern OpenGL (>= 3.3) with GLFW, GLAD, and GLM
- Supports both Windows and Linux using CMake

---

## 🛠️ Requirements

- CMake >= 3.8
- C++20 compiler (MSVC, GCC, or Clang)
- Visual Studio (on Windows) or build tools on Linux
- All third-party dependencies (GLAD, GLFW, GLM) are in `ThirdPartyLibraries/`

---

## 🔧 Build Instructions

```bash
# Create a build folder
mkdir out
cd out

# Configure the project
cmake ..

# Build the project
cmake --build .
```

---

## 🚀 Run

After building, run the generated `STLViewer` executable.  

---

## 📁 Project Structure

```
STLViewer/
├── STLViewer/                 # Source (.cpp/.h)
├── ThirdPartyLibraries/      # GLAD, GLFW, GLM
├── CMakeLists.txt            # Top-level CMake
├── README.md
├── .gitignore
└── out/                      # (build output folder)
```

## ✨ Credits

- [GLFW](https://www.glfw.org/)
- [GLAD](https://glad.dav1d.de/)
- [GLM](https://github.com/g-truc/glm)
