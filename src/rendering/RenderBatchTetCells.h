#ifndef RENDERBATCHTETCELLS_H
#define RENDERBATCHTETCELLS_H

#include "../commons/Shader.h"
#include "GLBuffers.h"
#include <unordered_set>
#include "../utils/Typedefs.h"

namespace MV
{
class RenderBatchTetCells
{
public:
    RenderBatchTetCells(TetrahedralMesh& mesh) : vbo(), ibo(), vao()
    {
        initFromMesh(mesh);
    }

    ~RenderBatchTetCells() {}

    void initFromMesh(TetrahedralMesh& mesh);

    void render(Shader& shader);

    template <typename Data>
    inline void setTet(int i, const std::vector<Data>& data)
    {
        assert(data.size()==12);
        updatedTets.insert(i);
        for (int j = 0; j < 12; ++j) vbo.set(12*i+j, data[j]);
    }

private:
    VBO<GL_FLOAT, float, 3, 3, 3, 3> vbo; // position, color, normal, tetcenter
    IBO<GL_TRIANGLES> ibo;
    VAO vao;

    std::unordered_set<int> updatedTets;

};
}

#endif // RENDERBATCHTETCELLS_H
