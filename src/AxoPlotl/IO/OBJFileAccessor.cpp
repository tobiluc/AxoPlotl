#include "OBJFileAccessor.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

bool AxoPlotl::IO::loadMeshOBJ(const std::string& filename, PolyhedralMesh &mesh)
{
    std::ifstream in(filename);
    if (!in.is_open()) {
        std::cerr << "Failed to open OBJ file: " << filename << std::endl;
        return false;
    }

    // Read line by line
    std::string line;
    while (std::getline(in, line)) {
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        // Vertex
        if (type == "v") {
            float x, y, z;
            iss >> x >> y >> z;
            mesh.add_vertex(OVM::Vec3d{x, y, z});
        }
        else
        {
            int dim = -1;
            if (type == "l") {dim = 1;}
            else if (type == "f") {dim = 2;}
            if (dim == -1) {continue;}

            std::vector<OVM::VH> vhs;
            std::string token;
            while (iss >> token) {
                size_t pos = token.find('/');
                if (pos != std::string::npos) {
                    token = token.substr(0, pos); // Only use vertex index (no other data)
                }
                int idx = std::stoi(token) - 1; // OBJ is 1-indexed
                vhs.push_back(OVM::VH(idx));
            }

            mesh.add_face(vhs);
        }
    }

    return true;
}
