#include "TetMeshRenderer.h"
#include "GLFW/glfw3.h"

namespace MV
{

TetMeshRenderer::TetMeshRenderer(TetrahedralMesh& mesh) :
    mesh(mesh),
    cellsBatch(TriangleBatch(4*mesh.n_cells())),
    facesBatch(TriangleBatch(mesh.n_faces())),
    boundaryFacesBatch(TriangleBatch(mesh_n_boundary_faces(mesh))),
    edgesBatch(LineBatch(mesh.n_edges())),
    triangleShader("../res/shaders/color.vert", "../res/shaders/color_triangles_default.geom", "../res/shaders/color_phong.frag"),
    lineShader("../res/shaders/color.vert", "../res/shaders/color_lines_default.geom", "../res/shaders/color_phong.frag")
{
    createCellsBatch(0.875f);
    createFacesBatch();
    createEdgesBatch();
}

void TetMeshRenderer::render(const glm::mat4x4& v, const glm::mat4x4& p)
{
    Shader shader = triangleShader;

    shader = triangleShader;
    shader.use();

    shader.setFloat("time", glfwGetTime());

    shader.setFloat("lineWidth", 5);

    shader.setMat4x4f("model_matrix", Mat4x4f(1.0));
    shader.setMat4x4f("view_matrix", v);
    shader.setMat4x4f("projection_matrix", p);

    shader.setVec3f("light.position", Vec3f(0,0,0));
    shader.setVec3f("light.ambient", Vec3f(0.5,0.5,0.5));
    shader.setVec3f("light.diffuse", Vec3f(0.3,0.3,0.3));
    shader.setVec3f("light.specular", Vec3f(0.2,0.2,0.2));

    cellsBatch.render();

    shader = lineShader;
    shader.use();

    shader.setFloat("time", glfwGetTime());

    shader.setFloat("lineWidth", 5);

    shader.setMat4x4f("model_matrix", Mat4x4f(1.0));
    shader.setMat4x4f("view_matrix", v);
    shader.setMat4x4f("projection_matrix", p);

    shader.setVec3f("light.position", Vec3f(0,0,0));
    shader.setVec3f("light.ambient", Vec3f(0.5,0.5,0.5));
    shader.setVec3f("light.diffuse", Vec3f(0.3,0.3,0.3));
    shader.setVec3f("light.specular", Vec3f(0.2,0.2,0.2));

    edgesBatch.render();
}
void TetMeshRenderer::createCellsBatch(float scale)
{
    MV::Color::Color col = {0.4,0.8f,0.4,1};
    for (auto c_it = mesh.c_iter(); c_it.is_valid(); ++c_it)
    {
        auto ch = *c_it;
        auto vhs = mesh.get_cell_vertices(ch);
        std::unordered_map<MV::VertexHandle, MV::OVM::Vec3d> p;
        for (auto vh : vhs) p[vh] = mesh.vertex(vh);
        MV::scaleTetrahedron(p.at(vhs[0]), p.at(vhs[1]), p.at(vhs[2]), p.at(vhs[3]), scale);
        auto hfhs = mesh.cell(ch).halffaces();
        for (auto hfh : hfhs)
        {
            auto vhs3 = mesh.get_halfface_vertices(hfh);
            auto n = (p.at(vhs3[2]) - p.at(vhs3[0])).cross(p.at(vhs3[1]) - p.at(vhs3[0])).normalize();
            auto v0 = toArray<float,10>(p.at(vhs3[0]), col, n);
            auto v1 = toArray<float,10>(p.at(vhs3[1]), col, n);
            auto v2 = toArray<float,10>(p.at(vhs3[2]), col, n);
            cellsBatch.addTriangle(v0, v1, v2);
        }
    }
}

void TetMeshRenderer::createFacesBatch()
{
    Random random;
    for (auto f_it = mesh.f_iter(); f_it.is_valid(); ++f_it)
    {
        auto fh = *f_it;
        auto vhs = mesh.get_halfface_vertices(fh.halfface_handle(0));
        auto p0 = mesh.vertex(vhs[0]);
        auto p1 = mesh.vertex(vhs[1]);
        auto p2 = mesh.vertex(vhs[2]);
        MV::Color::Color col = {random.randf(), random.randf(), random.randf(), 1.0f};
        auto v0 = toArray<float,10>(p0, col, Vec3f(0, 0, 1));
        auto v1 = toArray<float,10>(p1, col, Vec3f(0, 0, 1));
        auto v2 = toArray<float,10>(p2, col, Vec3f(0, 0, 1));
        facesBatch.addTriangle(v0, v1, v2);

        if (mesh.is_boundary(fh)) boundaryFacesBatch.addTriangle(v0, v1, v2);
    }
}

void TetMeshRenderer::createEdgesBatch()
{
    Random random;
    for (auto e_it = mesh.e_iter(); e_it.is_valid(); ++e_it)
    {
        auto eh = *e_it;
        auto p0 = mesh.vertex(mesh.edge(eh).from_vertex());
        auto p1 = mesh.vertex(mesh.edge(eh).to_vertex());
        MV::Color::Color col = {random.randf(), random.randf(), random.randf(), 1.0f};
        auto v0 = toArray<float,10>(p0, col, Vec3f(0, 0, 1));
        auto v1 = toArray<float,10>(p1, col, Vec3f(0, 0, 1));
        edgesBatch.addLine(v0, v1);
    }
}

}
