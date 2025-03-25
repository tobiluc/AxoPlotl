#include "TrianglesRenderBatch.h"
#include "../utils/Utils.h"
#include "../utils/Time.h"
#include "../commons/Shader.h"

namespace MV
{

TrianglesRenderBatch::TrianglesRenderBatch(size_t max_num_triangles)
{
    vao.generateNew();
    vbo.generateNew(3 * max_num_triangles);
    ibo.generateNew(3 * max_num_triangles);

    removeAll(max_num_triangles);

    vao.unbind();

    // Generate picking buffers
    vao_picking.generateNew();
    vbo.defineAttributes({0,3});

    assert(free.size() == max_num_triangles && num_elements() == 0);
}

TrianglesRenderBatch::TrianglesRenderBatch(TetrahedralMesh& mesh)
{
    initFromMesh(mesh);
}

void TrianglesRenderBatch::initFromMesh(TetrahedralMesh& mesh)
{
    uint nTriangles = mesh_n_boundary_faces(mesh);
    const uint nVertices = 3*nTriangles;
    uint nIndices = nVertices;

    vao.generateNew();
    vbo.generateNew(nVertices);
    ibo.generateNew(nIndices);

    removeAll(nTriangles);

    // create the vertex data
    auto prop = mesh.request_face_property<int>("AlgoHex::FeatureFaces");
    uint idx = 0;
    for (auto f_it = mesh.f_iter(); f_it.is_valid(); ++f_it)
    {
        auto fh = *f_it;
        if (!mesh.is_boundary(fh)) continue;
        //free.insert(idx);
        auto hfh = fh.halfface_handle(0); if (!mesh.incident_cell(hfh).is_valid()) hfh = hfh.opposite_handle();

        auto vhs = mesh.get_halfface_vertices(fh.halfface_handle(0));

        Color col = Color::COLORS[prop[fh]%Color::COLORS.size()];

        // Vertex Positions
        const auto& p0 = mesh.vertex(vhs[0]);
        const auto& p1 = mesh.vertex(vhs[1]);
        const auto& p2 = mesh.vertex(vhs[2]);

        add(Triangle(Point(p0, col), Point(p1, col), Point(p2, col)));

        idx++;
    }

    // Generate picking buffers
    vao_picking.generateNew();
    vbo.defineAttributes({0,3});

    assert(free.size() == 0 && num_elements() == max_num_elements());
}

void TrianglesRenderBatch::render()
{
    Shader::FACES_SHADER.use();

    // only rebuffer part of data that has changed
    vbo.bind();
    while (!updated.empty())
    {
        uint i = updated.extract(updated.begin()).value(); // pop
        vbo.bufferSubData(3*i, 3);
    }
    vao.bind();
    vbo.enableAttributes();

    ibo.drawAll();

    vbo.disableAttributes();
    vao.unbind();
}

void TrianglesRenderBatch::renderPicking(const glm::mat4x4& mvp, const uint batch_idx)
{
    Shader::PICKING_SHADER.use();

    Shader::PICKING_SHADER.setMat4x4f("model_view_projection_matrix", mvp);
    Shader::PICKING_SHADER.setUInt("mesh_index", batch_idx+1);

    vbo.bind();
    vao_picking.bind();
    vbo.enableAttributes({0,3}); // position and index

    ibo.drawAll();

    vbo.disableAttributes();
    vao_picking.unbind();
}

}
