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
    std::vector<std::array<float,3>> vertices;
    std::vector<std::array<unsigned int,2>> edges;
    std::vector<std::vector<unsigned int>> faces;

    while (std::getline(in, line)) {
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        // Vertex
        if (type == "v") {
            float x, y, z;
            iss >> x >> y >> z;
            vertices.push_back({x, y, z});
        }
        else if (type == "l") {
            unsigned int i, j;
            iss >> i >> j;
            edges.push_back({i-1, j-1});
        }
        else if (type == "f") {
            std::vector<OVM::VH> vhs;
            std::string token;
            faces.emplace_back();
            while (iss >> token) {
                size_t pos = token.find('/');
                if (pos != std::string::npos) {
                    token = token.substr(0, pos); // Only use vertex index (no other data)
                }
                faces.back().push_back(std::stoi(token)-1);
            }
            mesh.add_face(vhs);
        }
    }

    for (const auto& v : vertices) {
        mesh.add_vertex(OVM::Vec3d(v[0],v[1],v[2]));
    }
    for (const auto& e : edges) {
        mesh.add_edge(OVM::VH(e[0]),OVM::VH(e[1]));
    }
    for (const auto& f : faces) {
        std::vector<OVM::VH> vhs;
        for (unsigned int i : f) {vhs.emplace_back(i);}
        mesh.add_face(vhs);
    }

    return true;
}
