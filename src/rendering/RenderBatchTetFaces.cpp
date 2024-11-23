#include "RenderBatchTetFaces.h"

namespace MV
{

RenderBatchTetFaces::RenderBatchTetFaces(TetrahedralMesh& mesh) :
    val(),
    vertices(3*mesh.n_faces()*val.totalSize())
{
    // generate vertex array object
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Each face has 3 separate vertices to have correct normals
    uint nVertices = 3*mesh.n_faces();
    nIndices = 3*mesh.n_faces();

    // compute the constant triangle indices (012,345,678,91011,...)
    std::vector<uint> indices(nIndices);
    for (int i = 0; i < nIndices; ++i)
    {
        indices[i] = i;
    }

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

    // create the vertex data
    Random random;
    int i = 0;
    for (auto f_it = mesh.f_iter(); f_it.is_valid(); ++f_it)
    {
        auto fh = *f_it;
        if (!mesh.is_boundary(fh)) continue;
        auto vhs = mesh.get_halfface_vertices(fh.halfface_handle(0));
        auto p0 = mesh.vertex(vhs[0]);
        auto p1 = mesh.vertex(vhs[1]);
        auto p2 = mesh.vertex(vhs[2]);
        std::array<float,3> col = {random.randf(), random.randf(), random.randf()};
        auto v0 = toArray<float,9>(p0, col, Vec3f(0, 0, 1));
        auto v1 = toArray<float,9>(p1, col, Vec3f(0, 0, 1));
        auto v2 = toArray<float,9>(p2, col, Vec3f(0, 0, 1));

        setFace(i++, std::vector({v0, v1, v2}));
    }
}

RenderBatchTetFaces::~RenderBatchTetFaces()
{
    if (vbo) glDeleteBuffers(1, &vbo);
    if (ibo) glDeleteBuffers(1, &ibo);
    if (vao) glDeleteVertexArrays(1, &vao);
}

void RenderBatchTetFaces::render(Shader& shader)
{

    // only rebuffer part of data that has changed
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    while (!updatedFaces.empty())
    {
        int i = updatedFaces.extract(updatedFaces.begin()).value(); // pop
        glBufferSubData(GL_ARRAY_BUFFER, 3*i*val.totalSizeBytes(), 3*val.totalSizeBytes(), &vertices[3*i*val.totalSize()]);
    }

    glBindVertexArray(vao);
    for (int i = 0; i < val.numAttrs(); ++i) glEnableVertexAttribArray(i);

    glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, NULL);

    for (int i = 0; i < val.numAttrs(); ++i) glDisableVertexAttribArray(i);
    glBindVertexArray(0);
}

}
