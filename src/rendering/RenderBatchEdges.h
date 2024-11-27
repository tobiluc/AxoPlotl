#ifndef RENDERBATCHEDGES_H
#define RENDERBATCHEDGES_H

#include "../utils/Utils.h"
#include "../commons/Shader.h"
#include "GLBuffers.h"
#include <unordered_set>

namespace MV
{
class RenderBatchEdges
{
public:
    RenderBatchEdges(TetrahedralMesh& mesh) : vao()
    {
        initFromMesh(mesh);
    }

    ~RenderBatchEdges()
    {
        deleteBuffers();
    }

    void initFromMesh(TetrahedralMesh& mesh);

    void render(Shader& shader);

    template <typename Data>
    inline void setEdge(int i, const std::vector<Data>& data)
    {
        assert(data.size()==2);
        updatedEdges.insert(i);
        int n = val.totalSize();

        i *= (2*n);
        for (int k = 0; k < n; ++k)
        {
            vertices[i+(0*n)+k] = data[0][k];
            vertices[i+(1*n)+k] = data[1][k];
        }
    }

private:
    VAL<GL_FLOAT, float, 3, 3> val; // position, color
    std::vector<float> vertices;
    GLuint vao = 0, ibo = 0, vbo = 0;
    uint nIndices;
    std::unordered_set<int> updatedEdges;

    inline void deleteBuffers()
    {
        if (vbo) glDeleteBuffers(1, &vbo);
        if (ibo) glDeleteBuffers(1, &ibo);
        if (vao) glDeleteVertexArrays(1, &vao);
    }
};
}

#endif // RENDERBATCHEDGES_H
