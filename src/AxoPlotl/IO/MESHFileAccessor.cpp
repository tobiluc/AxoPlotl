#include "MESHFileAccessor.h"

bool AxoPlotl::IO::loadMeshMESH(const std::filesystem::path& filepath, PolyhedralMesh &mesh)
{
    std::ifstream in(filepath);
    if (!in) {
        std::cerr << "Error: Cannot open file " << filepath << "\n";
        return false;
    }

    std::string keyword;

    while (in >> keyword) {
        if (keyword == "MeshVersionFormatted") {
            std::string dummy;
            std::getline(in, dummy);
        }
        else if (keyword == "Dimension") {
            std::string dummy;
            std::getline(in, dummy);
        }
        else if (keyword == "Vertices") {
            size_t num_vertices;
            in >> num_vertices;
            mesh.reserve_vertices(num_vertices);
            for (int i = 0; i < num_vertices; ++i) {
                double x, y, z;
                int region;
                in >> x >> y >> z >> region;
                mesh.add_vertex(OVM::Vec3d(x, y, z));
            }
        }
        else if (keyword == "Tetrahedra") {
            size_t num_tets;
            in >> num_tets;
            mesh.reserve_cells(num_tets);
            for (int i = 0; i < num_tets; ++i) {
                std::vector<OVM::VertexHandle> vhs;
                for (int j = 0; j < 4; ++j) {
                    int tmp = 0;
                    in >> tmp;
                    vhs.push_back(OVM::VertexHandle(tmp-1));
                }
                int region;
                in >> region;

                // Add Tet Cell
                std::vector<OVM::HalfFaceHandle> hfhs;
                hfhs.push_back(mesh.add_face({vhs[0], vhs[1], vhs[2]}).halfface_handle(0));
                hfhs.push_back(mesh.add_face({vhs[0], vhs[2], vhs[3]}).halfface_handle(0));
                hfhs.push_back(mesh.add_face({vhs[0], vhs[3], vhs[1]}).halfface_handle(0));
                hfhs.push_back(mesh.add_face({vhs[1], vhs[3], vhs[2]}).halfface_handle(0));
                mesh.add_cell(hfhs);
            }
        }
        else if (keyword == "Hexahedra") {
            size_t num_hexes;
            in >> num_hexes;
            mesh.reserve_cells(num_hexes);
            for (int i = 0; i < num_hexes; ++i) {
                std::vector<VertexHandle> vhs;
                for (int j = 0; j < 8; ++j) {
                    int v;
                    in >> v;
                    vhs.push_back(OVM::VertexHandle(v-1));
                }
                int region;
                in >> region;

                // Add Hex Cell
                std::vector<OVM::HalfFaceHandle> hfhs;
                hfhs.push_back(mesh.add_face({vhs[0], vhs[1], vhs[2], vhs[3]}).halfface_handle(0)); // bottom
                hfhs.push_back(mesh.add_face({vhs[4], vhs[6], vhs[5], vhs[7]}).halfface_handle(0)); // top
                hfhs.push_back(mesh.add_face({vhs[0], vhs[3], vhs[7], vhs[4]}).halfface_handle(0)); // left
                hfhs.push_back(mesh.add_face({vhs[1], vhs[5], vhs[6], vhs[2]}).halfface_handle(0)); // right
                hfhs.push_back(mesh.add_face({vhs[0], vhs[4], vhs[5], vhs[1]}).halfface_handle(0)); // front
                hfhs.push_back(mesh.add_face({vhs[2], vhs[6], vhs[7], vhs[3]}).halfface_handle(0)); // back
                mesh.add_cell(hfhs);

            }
        }
        else if (keyword == "End") {
            std::string dummy;
            std::getline(in, dummy);
        }
        else {
            std::cerr << "Warning: Unknown Keyword '" << keyword << std::endl;
            std::string dummy;
            std::getline(in, dummy);
        }
    }

    in.close();
    return true;
}
