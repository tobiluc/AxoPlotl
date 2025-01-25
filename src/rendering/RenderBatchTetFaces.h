#ifndef RENDERBATCHTETFACES_H
#define RENDERBATCHTETFACES_H

#include "../utils/Utils.h"
#include "../commons/Shader.h"
#include "GLBuffers.h"
#include <unordered_set>

namespace MV
{
class RenderBatchTetFaces
{
public:
    RenderBatchTetFaces(TetrahedralMesh& mesh) :
        vbo_outlines(),
        ibo_outlines(),
        vao_outlines(),
        vbo(),
        ibo(),
        vao()
    {
        initFromMesh(mesh);
    }

    ~RenderBatchTetFaces()
    {

    }

    inline void deleteBuffers()
    {
        vbo.deleteBuffer();
        ibo.deleteBuffer();
        vao.deleteBuffer();
        vbo_outlines.deleteBuffer();
        ibo_outlines.deleteBuffer();
        vao_outlines.deleteBuffer();
        vbo_picking.deleteBuffer();
        ibo_picking.deleteBuffer();
        vao_picking.deleteBuffer();
    }

    void initFromMesh(TetrahedralMesh& mesh);

    void render();

    template <typename Data>
    inline void setFace(int i, const std::vector<Data>& data)
    {
        assert(data.size()==3);
        updatedFaces.insert(i);
        for (int j = 0; j < 3; ++j) vbo.set(3*i+j, data[j]);
    }
private:
    VBO<GL_FLOAT, float, 3, 3, 3> vbo; // position, color, normal
    IBO<GL_TRIANGLES> ibo;
    VAO vao;

    std::unordered_set<int> updatedFaces;

    VBO<GL_FLOAT, float, 3> vbo_outlines; // position
    IBO<GL_LINES> ibo_outlines;
    VAO vao_outlines;

    VBO<GL_FLOAT, float, 3> vbo_picking; // position
    IBO<GL_LINES> ibo_picking;
    VAO vao_picking;

};
}

#endif // RENDERBATCHTETFACES_H
