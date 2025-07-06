#include "LinesRenderBatch.h"
#include "../commons/Shader.h"
#include "../utils/Time.h"

namespace AxoPlotl::Rendering
{

LinesRenderBatch::LinesRenderBatch(size_t max_num_lines)
{
    vao.generateNew();
    vbo.generateNew(2 * max_num_lines);
    ibo.generateNew(2 * max_num_lines);

    removeAll(max_num_lines);

    vao.unbind();
}

LinesRenderBatch::LinesRenderBatch(TetrahedralMesh& mesh)
{
    initFromMesh(mesh);
}

void LinesRenderBatch::initFromMesh(TetrahedralMesh& mesh)
{
    uint nVertices = 2*mesh.n_edges();
    uint nIndices = nVertices;

    vao.generateNew();
    vbo.generateNew(nVertices);
    ibo.generateNew(nIndices);

    removeAll(mesh.n_edges());

    std::vector<GLuint> indices_features;

    // create the vertex data and indices arrays
    auto prop = mesh.template request_edge_property<int>("AlgoHex::FeatureEdges");
    for (auto e_it = mesh.e_iter(); e_it.is_valid(); ++e_it)
    {
        auto eh = *e_it;
        int i = eh.idx();
        auto heh = eh.halfedge_handle(0);
        auto vh0 = mesh.from_vertex_handle(heh);
        auto vh1 = mesh.to_vertex_handle(heh);
        Color col = Color::COLORS[prop[eh]%Color::COLORS.size()];

        const auto& p0 = mesh.vertex(vh0);
        const auto& p1 = mesh.vertex(vh1);

        add(Line(Point(p0,col),Point(p1,col)));

        if (prop[eh])
        {
            indices_features.push_back(2*i+0);
            indices_features.push_back(2*i+1);
        }
    }
    // ibo_features.generateNew(indices_features);
}

void LinesRenderBatch::render()
{
    Shader::EDGES_SHADER.use();

    // only rebuffer part of data that has changed
    vbo.bind();
    while (!updated.empty())
    {
        uint i = updated.extract(updated.begin()).value(); // pop
        vbo.bufferSubData(2*i, 2);
    }

    vao.bind();
    vbo.enableAttributes();

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-0.75f, -0.75f); // ensure the lines are drawn slightly in front

    ibo.drawAll();
    // if (num_visible_elements() == max_num_elements()) {ibo.drawAll();}
    // else {ibo.drawMultiElements(begin_offsets_bytes);}

    glDisable(GL_POLYGON_OFFSET_FILL);

    vbo.disableAttributes();
    vao.unbind();
}

// int LinesRenderBatch::add(const Line& l)
// {
//     if (free.empty()) return -1;

//     // Get the first free index
//     uint idx = free.extract(free.begin()).value();

//     // Add the index to the updated list
//     updated.insert(idx);

//     // Update the array of active indices
//     begin_offsets_bytes.push_back((void*)(2 * idx * sizeof(GLuint)));
//     index_in_begin_offsets[idx] = num_lines()-1;

//     // Store the data in the vbo
//     l.buffer(vbo, idx);

//     // Return the index
//     return idx;
// }

// void LinesRenderBatch::remove(uint idx)
// {
//     // Remove index from list of active indices
//     begin_offsets_bytes.erase(begin_offsets_bytes.begin() + index_in_begin_offsets[idx]);

//     // Shift indices of later points
//     for (uint idx2 = idx; idx2 < index_in_begin_offsets.size(); ++idx2)
//     {
//         index_in_begin_offsets[idx2] -= 1;
//     }

//     // Declare as updated and mark as free
//     updated.insert(idx);
//     free.insert(idx);

//     assert(free.size() + num_lines() == max_num_lines());
// }

}
