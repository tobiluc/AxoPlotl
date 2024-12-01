#include "RenderBatchHexCells.h"
#include "../utils/Utils.h"

namespace MV
{

void RenderBatchHexCells::initFromMesh(HexahedralMesh& mesh)
{
    vao.generateNew();
    vbo.generateNew(24*mesh.n_cells());

    // Indices define quads 012 023 456 467 ...
    std::vector<GLuint> indices(36*mesh.n_cells());
    for (int i = 0; i < 6*mesh.n_cells(); ++i)
    {
        indices[6*i+0] = 4*i+0;
        indices[6*i+1] = 4*i+1;
        indices[6*i+2] = 4*i+2;
        indices[6*i+3] = 4*i+2;
        indices[6*i+4] = 4*i+3;
        indices[6*i+5] = 4*i+0;
    }
    ibo.generateNew(indices);

    // create the vertex data
    MV::Color col = {0.4,0.8f,0.4};
    for (auto c_it = mesh.c_iter(); c_it.is_valid(); ++c_it)
    {
        auto ch = *c_it;
        auto hfhs = mesh.cell(ch).halffaces();

        //auto tetVhs = mesh.get
        auto hexcenter = HexahedralMesh::PointT(0,0,0);
        for (auto vh : mesh.cell_vertices(ch)) hexcenter += mesh.vertex(vh);
        hexcenter /= 8.f;

        const int n = 3+3+3+3;
        std::vector<std::array<float,n>> data;
        for (int i = 0; i < 4; ++i)
        {
            auto normal = mesh.normal(hfhs[i]);
            auto vhs = mesh.get_halfface_vertices(hfhs[i]);
            for (int j = 0; j < 3; ++j)
            {
                data.push_back(toArray<float,n>(mesh.vertex(vhs[j]), col, normal, hexcenter));
            }
        }
        setHex(ch.idx(), data);
    }

    vao.unbind();
}

void RenderBatchHexCells::render()
{
    MV::Shader::TET_CELLS_SHADER.use();

    vbo.bind();
    while (!updatedHexes.empty())
    {
        int i = updatedHexes.extract(updatedHexes.begin()).value(); // pop
        vbo.bufferSubData(24*i, 24);
    }

    vao.bind();
    vbo.enableAttributes();

    ibo.draw();

    vbo.disableAttributes();

    vao.unbind();
}

}
