#include "RenderBatchEdges.h"

namespace MV
{

RenderBatchEdges::RenderBatchEdges(TetrahedralMesh& mesh) :
    val(),
    vertices(2*mesh.n_edges()*val.totalSize())
{
    // create the vertex data and indices arrays
    nIndices = 2*mesh.n_edges();
    std::vector<uint> indices(nIndices);

    for (auto e_it = mesh.e_iter(); e_it.is_valid(); ++e_it)
    {
        auto eh = *e_it;
        int i = eh.idx();
        bool boundary = mesh.is_boundary(eh);
        int valence = mesh.valence(eh);
        auto heh = eh.halfedge_handle(0);
        auto vh0 = mesh.from_vertex_handle(heh);
        auto vh1 = mesh.to_vertex_handle(heh);

        float t = valence/10.f; if (t > 1) t = 1;
        Color col = Color({1-t,t,0});

        auto d0 = toArray<float,6>(mesh.vertex(vh0), col);
        auto d1 = toArray<float,6>(mesh.vertex(vh1), col);

        setEdge(i, std::vector({d0, d1}));

        indices[2*i+0] = 2*i+0;
        indices[2*i+1] = 2*i+1;
    }

    // generate vertex array object
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Each edge has 2 separate vertices to have correct colors
    uint nVertices = 2*mesh.n_edges();

    // generate vertex buffer object
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, nVertices * val.totalSizeBytes(), &vertices[0], GL_DYNAMIC_DRAW);

    // generate index buffer object
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), &indices[0], GL_STATIC_DRAW);

    // define attributes
    for (int i = 0; i < val.numAttrs(); ++i)
    {
        glVertexAttribPointer(i, val.attrSize(i), val.type(), GL_FALSE, val.totalSizeBytes(), (void*)(val.attrOffsetBytes(i)));
        glEnableVertexAttribArray(i);
    }
}

RenderBatchEdges::~RenderBatchEdges()
{
    if (vbo) glDeleteBuffers(1, &vbo);
    if (ibo) glDeleteBuffers(1, &ibo);
    if (vao) glDeleteVertexArrays(1, &vao);
}

void RenderBatchEdges::render(Shader& shader)
{

    // only rebuffer part of data that has changed
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    while (!updatedEdges.empty())
    {
        int i = updatedEdges.extract(updatedEdges.begin()).value(); // pop
        glBufferSubData(GL_ARRAY_BUFFER, 2*i*val.totalSizeBytes(), 2*val.totalSizeBytes(), &vertices[2*i*val.totalSize()]);
    }

    glBindVertexArray(vao);
    for (int i = 0; i < val.numAttrs(); ++i) glEnableVertexAttribArray(i);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glDrawElements(GL_LINES, nIndices, GL_UNSIGNED_INT, NULL);

    for (int i = 0; i < val.numAttrs(); ++i) glDisableVertexAttribArray(i);
    glBindVertexArray(0);
}

}
