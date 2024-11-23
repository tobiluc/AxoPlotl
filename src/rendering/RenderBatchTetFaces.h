#ifndef RENDERBATCHTETFACES_H
#define RENDERBATCHTETFACES_H

#include "../utils/Utils.h"
#include "../commons/Shader.h"
#include "VertexAttributesLayout.h"
#include <unordered_set>

namespace MV
{
class RenderBatchTetFaces
{
public:
    RenderBatchTetFaces(TetrahedralMesh& mesh);
    ~RenderBatchTetFaces();

    void render(Shader& shader);

    template <typename Data>
    inline void setFace(int i, const std::vector<Data>& data)
    {
        assert(data.size()==3);
        updatedFaces.insert(i);
        int n = val.totalSize();

        i *= (3*n);
        for (int j = 0; j < 3; ++j)
        {
            for (int k = 0; k < n; ++k)
            {
                vertices[i+(j*n)+k] = data[j][k];
            }
        }
    }

    template <typename Col>
    inline void setFaceColor(int i, const Col& col)
    {
        updatedFaces.insert(i);
        i *= (3*val.totalSize());
        i += 3; // color offset
        vertices[i+0] = col[0];
        vertices[i+1] = col[1];
        vertices[i+2] = col[2];
        int n = val.totalSize();
    }

private:
    VertexAttributesLayout<GL_FLOAT, float, 3, 3, 3> val; // position, color, normal
    std::vector<float> vertices;
    GLuint vao, ibo, vbo;
    uint nIndices;
    std::unordered_set<int> updatedFaces;

};
}

#endif // RENDERBATCHTETFACES_H
