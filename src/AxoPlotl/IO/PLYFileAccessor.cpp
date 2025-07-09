#include "PLYFileAccessor.h"
#include <fstream>
#include <sstream>
#include <iostream>

enum PLYFormat {
    PLY_NONE, PLY_ASCII, PLY_BINARY_LITTLE_ENDIAN, PLY_BINARY_BIG_ENDIAN
};

bool AxoPlotl::IO::loadMeshPLY(const std::string& filename, Mesh& mesh)
{
    // Open File
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open PLY file: " << filename << "\n";
        return false;
    }

    // Header
    std::string line;
    uint numVertices = 0, numFaces = 0;
    PLYFormat format = PLY_NONE;
    bool headerStatus = false;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string word;
        iss >> word;

        if (word == "element") {
            std::string type;
            iss >> type;
            if (type == "vertex") {
                iss >> numVertices;
            } else if (type == "face") {
                iss >> numFaces;
            }
        }
        else if (word == "format") {
            std::string f;
            double version;
            iss >> f >> version;
            if (f == "ascii") {format = PLY_ASCII;}
            else if (f == "binary_little_endian") {format = PLY_BINARY_LITTLE_ENDIAN;}
            else if (f == "binary_big_endian") {format = PLY_BINARY_BIG_ENDIAN;}
        }
        else if (word == "property") {
            // TODO
        }
        else if (word == "end_header") {
            headerStatus = true;
            break;
        }
    }
    if (!headerStatus || format == PLY_NONE) {
        std::cerr << "PLY: Invalid or incomplete PLY header" << std::endl;
        return false;
    }

    if (format == PLY_ASCII) {

        // Vertices
        for (uint i = 0; i < numVertices; ++i) {
            if (!std::getline(file, line)) break;
            std::istringstream iss(line);
            float x, y, z;
            iss >> x >> y >> z;
            mesh.addVertex(Vec3f(x, y, z));
        }

        // Faces
        for (uint i = 0; i < numFaces; ++i) {
            if (!std::getline(file, line)) break;
            std::istringstream iss(line);
            int n;
            iss >> n;
            std::vector<int> indices(n);
            for (int j = 0; j < n; ++j) {
                iss >> indices[j];
            }
            mesh.addCell(2, indices);
        }

        return true;

    }

    if (format == PLY_BINARY_LITTLE_ENDIAN) {

        // Vertices
        for (uint i = 0; i < numVertices; ++i) {
            float coords[3];
            file.read(reinterpret_cast<char*>(coords), sizeof(coords));
            if (!file) {
                std::cerr << "PLY: Error reading vertex " << i << std::endl;
                return false;
            }
            mesh.addVertex(Vec3f(coords[0], coords[1], coords[2]));
        }

        // Faces
        for (uint i = 0; i < numFaces; ++i) {
            uint8_t numVerts;
            file.read(reinterpret_cast<char*>(&numVerts), sizeof(uint8_t));
            if (!file) {
                std::cerr << "PLY: Error reading face size" << std::endl;
                return false;
            }

            std::vector<int> indices(numVerts);
            file.read(reinterpret_cast<char*>(indices.data()), numVerts * sizeof(int));
            if (!file) {
                std::cerr << "PLY: Error reading face indices" << std::endl;
                return false;
            }

            mesh.addCell(2, indices);
        }

        std::cout << mesh.numVertices() << "   " << mesh.numCells() << std::endl;

        return true;
    }

    if (format == PLY_BINARY_BIG_ENDIAN) {
        std::cerr << "PLY: Bin endian is unsupported" << std::endl; // TODO
    }

    return false;
}
