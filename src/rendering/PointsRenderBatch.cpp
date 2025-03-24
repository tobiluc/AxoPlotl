#include "PointsRenderBatch.h"
#include "../commons/Shader.h"
#include "../utils/Globals.h"

namespace MV
{

PointsRenderBatch::PointsRenderBatch(size_t max_num_points)
{
    vao.generateNew();
    vbo.generateNew(max_num_points);
    ibo.generateNew(max_num_points);

    removeAll(max_num_points);

    vao.unbind();
}

PointsRenderBatch::PointsRenderBatch(TetrahedralMesh& mesh)
{
    initFromMesh(mesh);
}

void PointsRenderBatch::initFromMesh(TetrahedralMesh& mesh)
{
    uint max_num_points = mesh.n_vertices();

    vao.generateNew();
    vbo.generateNew(max_num_points);
    ibo.generateNew(max_num_points);

    removeAll(max_num_points);

    auto prop = mesh.template request_vertex_property<int>("AlgoHex::FeatureVertices");
    for (auto v_it = mesh.v_iter(); v_it.is_valid(); ++v_it)
    {
        auto vh = *v_it;
        const auto& pos = mesh.vertex(vh);
        Color col = COLORS[prop[vh]%COLORS.size()];
        add(Point(pos, col));
    }

    vao.unbind();
}

void PointsRenderBatch::render()
{
    Shader::VERTICES_SHADER.use();

    vbo.bind();
    while (!updated.empty())
    {
        GLuint i = updated.extract(updated.begin()).value(); // pop
        vbo.bufferSubData(i, 1);
    }

    vao.bind();
    vbo.enableAttributes();

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-1.0f, -1.0f); // ensure the vertices are drawn slightly in front

    ibo.drawAll();

    glDisable(GL_POLYGON_OFFSET_FILL);

    vbo.disableAttributes();

    vao.unbind();
}

}
