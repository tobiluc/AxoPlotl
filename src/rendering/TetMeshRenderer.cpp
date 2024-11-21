#include "TetMeshRenderer.h"

namespace MV
{

TetMeshRenderer::TetMeshRenderer(TetrahedralMesh& mesh, Shader& shader) :
    mesh(mesh),
    cellsBatch(TriangleBatch(4*mesh.n_cells(), shader)),
    facesBatch(TriangleBatch(mesh.n_faces(), shader)),
    boundaryFacesBatch(TriangleBatch(mesh_n_boundary_faces(mesh), shader)),
    meshMode(MeshRenderMode::BOUNDARY_FACES)
{

    // add cells of mesh to batch as scaled down tets
    cellsBatch.initialize();
    MV::Color::Color col = {0.4,0.8f,0.4,1};
    for (auto c_it = mesh.c_iter(); c_it.is_valid(); ++c_it)
    {
        auto ch = *c_it;
        auto vhs = mesh.get_cell_vertices(ch);
        std::unordered_map<MV::VertexHandle, MV::OVM::Vec3d> p;
        for (auto vh : vhs) p[vh] = mesh.vertex(vh);
        MV::scaleTetrahedron(p.at(vhs[0]), p.at(vhs[1]), p.at(vhs[2]), p.at(vhs[3]), 0.9);
        auto hfhs = mesh.cell(ch).halffaces();
        for (auto hfh : hfhs)
        {
            auto vhs3 = mesh.get_halfface_vertices(hfh);
            auto n = (p.at(vhs3[2]) - p.at(vhs3[0])).cross(p.at(vhs3[1]) - p.at(vhs3[0])).normalize();
            MV::VertexData v0 = MV::vertexData(p.at(vhs3[0]), col, n);
            MV::VertexData v1 = MV::vertexData(p.at(vhs3[1]), col, n);
            MV::VertexData v2 = MV::vertexData(p.at(vhs3[2]), col, n);
            cellsBatch.addTriangle(v0, v1, v2);
        }
    }

    // add faces of mesh to batch as triangles with random color
    facesBatch.initialize();
    boundaryFacesBatch.initialize();
    Random random;
    for (auto f_it = mesh.f_iter(); f_it.is_valid(); ++f_it)
    {
        auto fh = *f_it;
        auto vhs = mesh.get_halfface_vertices(fh.halfface_handle(0));
        if (vhs.size()!=3 || !vhs[0].is_valid() || !vhs[1].is_valid() || !vhs[2].is_valid()) {std::cout << "Skip fh " << fh << std::endl; continue;}
        auto p0 = mesh.vertex(vhs[0]);
        auto p1 = mesh.vertex(vhs[1]);
        auto p2 = mesh.vertex(vhs[2]);
        MV::Color::Color col = {random.randf(), random.randf(), random.randf(), 1.0f};
        MV::VertexData v0 = MV::vertexData(p0, col, Vec3f(0, 0, 1));
        MV::VertexData v1 = MV::vertexData(p1, col, Vec3f(0, 0, 1));
        MV::VertexData v2 = MV::vertexData(p2, col, Vec3f(0, 0, 1));
        facesBatch.addTriangle(v0, v1, v2);

        if (mesh.is_boundary(fh)) boundaryFacesBatch.addTriangle(v0, v1, v2);
    }
}

void TetMeshRenderer::render()
{
    switch (meshMode) {
    case CELLS: cellsBatch.render(); break;
    case FACES: facesBatch.render(); break;
    case BOUNDARY_FACES: boundaryFacesBatch.render(); break;
    default: break;
    }
}

}
