#ifndef RENDERBATCHTETCELLS_H
#define RENDERBATCHTETCELLS_H

#include "../utils/Utils.h"
#include "../commons/Shader.h"
#include "VertexAttributesLayout.h"
#include <unordered_set>

namespace MV
{
class RenderBatchTetCells
{
public:
    RenderBatchTetCells(TetrahedralMesh& mesh);
    ~RenderBatchTetCells();

    void render(Shader& shader);

    template <typename Data>
    inline void setTet(int i, const std::vector<Data>& data)
    {
        assert(data.size()==12);
        updatedTets.insert(i);
        int n = val.totalSize();

        i *= (12*n);
        for (int j = 0; j < 12; ++j)
        {
            for (int k = 0; k < n; ++k)
            {
                vertices[i+(j*n)+k] = data[j][k];
            }
        }
    }

    template <typename Col>
    inline void setTetColor(int i, const Col& col)
    {
        updatedTets.insert(i);
        i *= (12*val.totalSize());
        i += 3; // color offset
        vertices[i+0] = col[0];
        vertices[i+1] = col[1];
        vertices[i+2] = col[2];
        int n = val.totalSize();
    }

private:
    VertexAttributesLayout<GL_FLOAT, float, 3, 3, 3, 3> val; // position, color, normal, tetcenter
    std::vector<float> vertices;
    GLuint vao, ibo, vbo;
    uint nIndices;
    std::unordered_set<int> updatedTets;

};
}

#endif // RENDERBATCHTETCELLS_H
