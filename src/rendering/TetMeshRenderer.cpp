#include "TetMeshRenderer.h"
#include "GLFW/glfw3.h"

namespace MV
{

TetMeshRenderer::TetMeshRenderer(TetrahedralMesh& mesh) :
    mesh(mesh),
    cellScale(0.9f),
    tetCellsBatch(mesh),
    facesBatch(mesh),
    edgesBatch(LineBatch(mesh.n_edges())),
    facesShader("../res/shaders/faces.vert", "../res/shaders/faces_phong.frag"),
    tetCellsShader("../res/shaders/cells.vert", "../res/shaders/cells_phong.frag"),
    lineShader("../res/shaders/color.vert", "../res/shaders/color_lines_width.geom", "../res/shaders/color_phong.frag"),
    light({Vec3f(0.5,0.5,0.5), Vec3f(0.3,0.3,0.3), Vec3f(0.2,0.2,0.2)})
{
    createEdgesBatch();
}

void TetMeshRenderer::render(const glm::mat4x4& v, const glm::mat4x4& p)
{
    if (showCells)
    {
        auto& shader = tetCellsShader;
        shader.use();

        shader.setFloat("time", glfwGetTime());

        shader.setFloat("cell_scale", cellScale);

        shader.setMat4x4f("model_matrix", Mat4x4f(1.0));
        shader.setMat4x4f("view_matrix", v);
        shader.setMat4x4f("projection_matrix", p);

        shader.setVec3f("light.position", Vec3f(0,0,0));
        shader.setVec3f("light.ambient", light.ambient);
        shader.setVec3f("light.diffuse", light.diffuse);
        shader.setVec3f("light.specular", light.specular);

        tetCellsBatch.render(shader);
    }
    if (showFaces)
    {
        auto& shader = facesShader;
        shader.use();

        shader.setFloat("time", glfwGetTime());

        shader.setFloat("lineWidth", 5);

        shader.setMat4x4f("model_matrix", Mat4x4f(1.0));
        shader.setMat4x4f("view_matrix", v);
        shader.setMat4x4f("projection_matrix", p);

        shader.setVec3f("light.position", Vec3f(0,0,0));
        shader.setVec3f("light.ambient", light.ambient);
        shader.setVec3f("light.diffuse", light.diffuse);
        shader.setVec3f("light.specular", light.specular);

        facesBatch.render(shader);
    }
    if (showEdges)
    {
        auto& shader = lineShader;
        shader.use();

        shader.setFloat("time", glfwGetTime());

        shader.setFloat("lineWidth", 5);

        shader.setMat4x4f("model_matrix", Mat4x4f(1.0));
        shader.setMat4x4f("view_matrix", v);
        shader.setMat4x4f("projection_matrix", p);

        shader.setVec3f("light.position", Vec3f(0,0,0));
        shader.setVec3f("light.ambient", light.ambient);
        shader.setVec3f("light.diffuse", light.diffuse);
        shader.setVec3f("light.specular", light.specular);

        edgesBatch.render();
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
        std::array<float,4> col = {random.randf(), random.randf(), random.randf(), 1.0f};
        auto v0 = toArray<float,10>(p0, col, Vec3f(0, 0, 1));
        auto v1 = toArray<float,10>(p1, col, Vec3f(0, 0, 1));
        edgesBatch.addLine(v0, v1);
    }
}

}
