#include "RenderBatchTetCells.h"

namespace MV
{

RenderBatchTetCells::RenderBatchTetCells(TetrahedralMesh& mesh) :
    val(),
    vertices(12*mesh.n_cells()*val.totalSize())
{
    // generate vertex array object
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Each tet has 4 triangular faces, 12 vertices to have correct normals
    uint nVertices = 12*mesh.n_cells();
    nIndices = 12*mesh.n_cells();

    // compute the constant triangle tetIndices (012,345,678,91011,...)
    std::vector<uint> indices(nIndices);
    for (int i = 0; i < nIndices; ++i) {indices[i] = i;}

    // generate vertex buffer object
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, nVertices * val.totalSizeBytes(), &vertices[0], GL_DYNAMIC_DRAW);

    // generate index buffer object
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, nIndices * sizeof(uint), &indices[0], GL_STATIC_DRAW);

    // define attributes
    for (int i = 0; i < val.numAttrs(); ++i)
    {
        glVertexAttribPointer(i, val.attrSize(i), val.type(), GL_FALSE, val.totalSizeBytes(), (void*)(val.attrOffsetBytes(i)));
        glEnableVertexAttribArray(i);
    }

    // create the vertex data
    MV::Color col = {0.4,0.8f,0.4};
    for (auto c_it = mesh.c_iter(); c_it.is_valid(); ++c_it)
    {
        auto ch = *c_it;
        auto hfhs = mesh.cell(ch).halffaces();

        auto tetVhs = mesh.get_cell_vertices(ch);
        auto incenter = tetIncenter(mesh.vertex(tetVhs[0]),mesh.vertex(tetVhs[1]),mesh.vertex(tetVhs[2]),mesh.vertex(tetVhs[3]));

        std::vector<std::array<float,3+3+3+3>> data;
        for (int i = 0; i < 4; ++i)
        {
            auto normal = mesh.normal(hfhs[i]);
            auto vhs = mesh.get_halfface_vertices(hfhs[i]);
            for (int j = 0; j < 3; ++j)
            {
                data.push_back(toArray<float,3+3+3+3>(mesh.vertex(vhs[j]), col, normal, incenter));
            }
        }
        setTet(ch.idx(), data);
    }
}

RenderBatchTetCells::~RenderBatchTetCells()
{
    if (vbo) glDeleteBuffers(1, &vbo);
    if (ibo) glDeleteBuffers(1, &ibo);
    if (vao) glDeleteVertexArrays(1, &vao);
}

void RenderBatchTetCells::render(Shader& shader)
{

    // only rebuffer part of data that has changed
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    while (!updatedTets.empty())
    {
        int i = updatedTets.extract(updatedTets.begin()).value(); // pop
        glBufferSubData(GL_ARRAY_BUFFER, 12*i*val.totalSizeBytes(), 12*val.totalSizeBytes(), &vertices[12*i*val.totalSize()]);
    }

    glBindVertexArray(vao);
    for (int i = 0; i < val.numAttrs(); ++i) glEnableVertexAttribArray(i);

    // Draw tets
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, NULL);

    for (int i = 0; i < val.numAttrs(); ++i) glDisableVertexAttribArray(i);
    glBindVertexArray(0);
}

// void RenderBatchTetCells::renderOutlines(Shader& shader)
// {
//     // only rebuffer part of data that has changed
//     glBindBuffer(GL_ARRAY_BUFFER, vbo);
//     while (!updatedTets.empty())
//     {
//         int i = updatedTets.extract(updatedTets.begin()).value(); // pop
//         glBufferSubData(GL_ARRAY_BUFFER, 12*i*val.totalSizeBytes(), 12*val.totalSizeBytes(), &vertices[12*i*val.totalSize()]);
//     }

//     glBindVertexArray(vao);
//     for (int i = 0; i < val.numAttrs(); ++i) glEnableVertexAttribArray(i);

//     glEnable(GL_POLYGON_OFFSET_FILL);
//     glPolygonOffset(-1.0f, -1.0f);
//     glDepthMask(GL_FALSE);

//     shader.setVec3f("color", Vec3f(0,0,0));
//     shader.setFloat("width", 2.0f);
//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
//     glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, NULL);

//     glDisable(GL_POLYGON_OFFSET_FILL);
//     glDepthMask(GL_TRUE);

//     for (int i = 0; i < val.numAttrs(); ++i) glDisableVertexAttribArray(i);
//     glBindVertexArray(0);
// }

}
