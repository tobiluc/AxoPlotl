#include "TrianglesRenderBatch.h"
#include "../utils/Utils.h"
#include "../utils/Globals.h"

namespace MV
{

void Triangle::computeNormal()
{
    // Vectors from p0
    std::array<float, 3> v0 = {vertices[1].position[0] - vertices[0].position[0],
                               vertices[1].position[1] - vertices[0].position[1],
                               vertices[1].position[2] - vertices[0].position[2]};

    std::array<float, 3> v1 = {vertices[2].position[0] - vertices[0].position[0],
                               vertices[2].position[1] - vertices[0].position[1],
                               vertices[2].position[2] - vertices[0].position[2]};

    // Compute cross product v0 × v1
    normal[0] = v0[1] * v1[2] - v0[2] * v1[1];
    normal[1] = v0[2] * v1[0] - v0[0] * v1[2];
    normal[2] = v0[0] * v1[1] - v0[1] * v1[0];

    // Normalize the normal vector
    float length = std::sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
    if (length > 0.0f) {
        normal[0] /= length;
        normal[1] /= length;
        normal[2] /= length;
    }
}

TrianglesRenderBatch::TrianglesRenderBatch(size_t max_num_triangles)
{
    vao.generateNew();
    vbo.generateNew(3 * max_num_triangles);
    ibo.generateNew(3 * max_num_triangles);

    for (uint i = 0; i < max_num_triangles; ++i)
    {
        free.insert(i);
    }

    vao.unbind();

    // GENERATE OUTLINES
    // vao_outlines.generateNew();
    // vbo.defineAttributes({0});

    // uint nIndices = 6*max_num_triangles;
    // std::vector<GLuint> indices_outlines(nIndices);
    // for (int i = 0; i < max_num_triangles; ++i)
    // {
    //     indices_outlines[6*i+0] = 3*i+0;
    //     indices_outlines[6*i+1] = 3*i+1;
    //     indices_outlines[6*i+2] = 3*i+1;
    //     indices_outlines[6*i+3] = 3*i+2;
    //     indices_outlines[6*i+4] = 3*i+2;
    //     indices_outlines[6*i+5] = 3*i+0;
    // };
    // ibo_outlines.generateNew(indices_outlines);

    // Generate picking buffers
    vao_picking.generateNew();
    vbo.defineAttributes({0,3});
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

    // create the vertex data
    auto prop = mesh.request_face_property<int>("AlgoHex::FeatureFaces");
    uint idx = 0;
    for (auto f_it = mesh.f_iter(); f_it.is_valid(); ++f_it)
    {
        free.insert(idx);

        auto fh = *f_it;
        if (!mesh.is_boundary(fh)) continue;
        auto hfh = fh.halfface_handle(0); if (!mesh.incident_cell(hfh).is_valid()) hfh = hfh.opposite_handle();
        auto normal = mesh.normal(hfh);
        auto vhs = mesh.get_halfface_vertices(fh.halfface_handle(0));

        Color col = COLORS[prop[fh]%COLORS.size()];

        // Vertex Positions
        const auto& p0 = mesh.vertex(vhs[0]);
        const auto& p1 = mesh.vertex(vhs[1]);
        const auto& p2 = mesh.vertex(vhs[2]);

        add(Triangle(Point(p0, col), Point(p1, col), Point(p2, col)));

        idx++;
    }

    // GENERATE OUTLINES
    // vao_outlines.generateNew();
    // vbo.defineAttributes({0});

    // nIndices = 6*nTriangles;
    // std::vector<GLuint> indices_outlines(nIndices);
    // for (int i = 0; i < nTriangles; ++i)
    // {
    //     indices_outlines[6*i+0] = 3*i+0;
    //     indices_outlines[6*i+1] = 3*i+1;
    //     indices_outlines[6*i+2] = 3*i+1;
    //     indices_outlines[6*i+3] = 3*i+2;
    //     indices_outlines[6*i+4] = 3*i+2;
    //     indices_outlines[6*i+5] = 3*i+0;
    // };
    // ibo_outlines.generateNew(indices_outlines);

    // Generate picking buffers
    vao_picking.generateNew();
    vbo.defineAttributes({0,3});
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

    ibo.draw();

    vbo.disableAttributes();
    vao.unbind();

    // Outlines

    // Shader::FACES_OUTLINES_SHADER.use();

    // glEnable(GL_POLYGON_OFFSET_FILL);
    // glPolygonOffset(-0.75f, -0.75f); // ensure the outline is drawn slightly in front

    // vbo.bind();
    // vao_outlines.bind();
    // vbo.enableAttributes({0});

    // ibo_outlines.draw();

    // vbo.disableAttributes();
    // vao_outlines.unbind();

    // glDisable(GL_POLYGON_OFFSET_FILL);
}

void TrianglesRenderBatch::renderPicking(const glm::mat4x4& mvp, const uint batch_idx)
{
    Shader::PICKING_SHADER.use();

    Shader::PICKING_SHADER.setMat4x4f("model_view_projection_matrix", mvp);
    Shader::PICKING_SHADER.setUInt("mesh_index", batch_idx+1);

    vbo.bind();
    vao_picking.bind();
    vbo.enableAttributes({0,3}); // position and index

    ibo.draw();

    vbo.disableAttributes();
    vao_picking.unbind();
}

int TrianglesRenderBatch::add(const Triangle& t)
{
    // If the batch is full, return -1
    if (free.empty()) return -1;

    // Retrieve the first free index
    uint idx = *free.begin();
    free.erase(free.begin());

    // Add the index to the updated list
    updated.insert(idx);

    // Store the data in the vbo
    t.buffer(vbo, idx);

    // Return the index
    return idx;
}

void TrianglesRenderBatch::remove(uint idx)
{
    // TODO
}

}
