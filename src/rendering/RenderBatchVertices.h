#ifndef RENDERBATCHVERTICES_H
#define RENDERBATCHVERTICES_H

#include "../utils/Utils.h"
#include "../commons/Shader.h"
#include "GLBuffers.h"
#include <unordered_set>

namespace MV
{
template<typename MeshT>
class RenderBatchVertices
{
public:

    //template<typename MeshT>
    RenderBatchVertices(MeshT& mesh) : vao()
    {
        initFromMesh(mesh);
    }

    ~RenderBatchVertices() {}

    void initFromMesh(MeshT& mesh);

    void render();

    template <typename Data>
    inline void setVertex(int i, const Data& data)
    {
        updatedVertices.insert(i);
        vbo.set(i, data);
    }

private:
    VBO<GL_FLOAT, float, 3, 3> vbo; // position, color
    IBO<GL_POINTS> ibo;
    VAO vao;
    std::unordered_set<int> updatedVertices;
};
}

#endif // RENDERBATCHVERTICES_H
