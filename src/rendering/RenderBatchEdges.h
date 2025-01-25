#ifndef RENDERBATCHEDGES_H
#define RENDERBATCHEDGES_H

#include "../utils/Utils.h"
#include "../commons/Shader.h"
#include "GLBuffers.h"
#include <unordered_set>

namespace MV
{
template<typename MeshT>
class RenderBatchEdges
{
public:

    //template<typename MeshT>
    RenderBatchEdges(MeshT& mesh) :
        vao(),
        vbo(),
        ibo(),
        ibo_features()
    {
        initFromMesh(mesh);
    }

    ~RenderBatchEdges()
    {

    }

    inline void deleteBuffers()
    {
        vbo.deleteBuffer();
        ibo.deleteBuffer();
        vao.deleteBuffer();
        ibo_features.deleteBuffer();
    }

    void initFromMesh(MeshT& mesh);

    void render();

    template <typename Data>
    inline void setEdge(int i, const std::vector<Data>& data)
    {
        assert(data.size()==2);
        updatedEdges.insert(i);
        vbo.set(2*i+0, data[0]);
        vbo.set(2*i+1, data[1]);
    }

private:
    VBO<GL_FLOAT, float, 3, 3> vbo; // position, color
    IBO<GL_LINES> ibo;
    IBO<GL_LINES> ibo_features;
    VAO vao;
    std::unordered_set<int> updatedEdges;
};
}

#endif // RENDERBATCHEDGES_H
