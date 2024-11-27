#ifndef RENDERBATCHHEXCELLS_H
#define RENDERBATCHHEXCELLS_H

#include "../commons/Shader.h"
#include "GLBuffers.h"
#include <unordered_set>
#include "../utils/Typedefs.h"

namespace MV
{
class RenderBatchHexCells
{
public:
    RenderBatchHexCells(HexahedralMesh& mesh) : vbo(), ibo(), vao()
    {
        initFromMesh(mesh);
    }

    ~RenderBatchHexCells() {}

    void initFromMesh(HexahedralMesh& mesh);

    void render(Shader& shader);

    template <typename Data>
    inline void setHex(int i, const std::vector<Data>& data)
    {
        assert(data.size()==24);
        updatedHexes.insert(i);
        for (int j = 0; j < 24; ++j) vbo.set(24*i+j, data[j]);
    }

private:
    VBO<GL_FLOAT, float, 3, 3, 3, 3> vbo; // position, color, normal, hexcenter
    IBO<GL_TRIANGLES> ibo;
    VAO vao;

    std::unordered_set<int> updatedHexes;

};
}


#endif // RENDERBATCHHEXCELLS_H
