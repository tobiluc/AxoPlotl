#include "RenderBatchTetCells.h"
#include "../utils/Utils.h"

namespace MV
{

void RenderBatchTetCells::initFromMesh(TetrahedralMesh& mesh)
{
    vao.generateNew();
    vbo.generateNew(12*mesh.n_cells());
    ibo.generateNew(12*mesh.n_cells());

    // create the vertex data
    MV::Color col = {0.4,0.8f,0.4};
    for (auto c_it = mesh.c_iter(); c_it.is_valid(); ++c_it)
    {
        auto ch = *c_it;
        auto hfhs = mesh.cell(ch).halffaces();

        auto tetVhs = mesh.get_cell_vertices(ch);
        auto incenter = tetIncenter(mesh.vertex(tetVhs[0]),mesh.vertex(tetVhs[1]),mesh.vertex(tetVhs[2]),mesh.vertex(tetVhs[3]));

        const int n = 3+3+3+3;
        std::vector<std::array<float,n>> data;
        for (int i = 0; i < 4; ++i)
        {
            auto normal = mesh.normal(hfhs[i]);
            auto vhs = mesh.get_halfface_vertices(hfhs[i]);
            for (int j = 0; j < 3; ++j)
            {
                data.push_back(toArray<float,n>(mesh.vertex(vhs[j]), col, normal, incenter));
            }
        }
        setTet(ch.idx(), data);
    }

    vao.unbind();
}

void RenderBatchTetCells::render()
{
    Shader::TET_CELLS_SHADER.use();

    vbo.bind();
    while (!updatedTets.empty())
    {
        int i = updatedTets.extract(updatedTets.begin()).value(); // pop
        vbo.bufferSubData(12*i, 12);
    }

    vao.bind();
    vbo.enableAttributes();

    ibo.drawAll();

    vbo.disableAttributes();

    vao.unbind();
}

}
