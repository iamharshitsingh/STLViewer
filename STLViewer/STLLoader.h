#pragma once
#include <string>
//#include <memory>
#include <fstream>
#include <iostream>
#include "Mesh.h"

class STLLoader {
public:
    static std::shared_ptr<Mesh> load(const std::string& filename);
};