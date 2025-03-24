#ifndef TRIANGLESRENDERBATCH_H
#define TRIANGLESRENDERBATCH_H

#include "../commons/Shader.h"
#include "GLBuffers.h"
#include "PointsRenderBatch.h"
#include <unordered_set>

namespace MV {

struct Triangle
{
    using VBO = VBO<GL_FLOAT, float, 3, 3, 3, 1>; // position, color, normal, index

    std::array<Point, 3> vertices;
    std::array<float, 3> normal;

    Triangle(const Point& p0, const Point& p1, const Point& p2) :
        vertices{p0, p1, p2}
    {
        computeNormal();
    }

    inline void buffer(VBO& vbo, uint idx) const
    {
        for (unsigned char i = 0; i < 3; ++i)
        {
            std::array<float, 10> data = {
                vertices[i].position[0],vertices[i].position[1],vertices[i].position[2],
                vertices[i].color[0],vertices[i].color[1],vertices[i].color[2],
                normal[0], normal[1], normal[2],
                (float)(idx+1)
            };
            vbo.set(3*idx+i, data);
        }
    }

private:
    void computeNormal();
};

class TrianglesRenderBatch
{
private:
    std::unordered_set<uint> updated;
    std::unordered_set<uint> free;

    Triangle::VBO vbo; // position, color, normal, element index
    IBO<GL_TRIANGLES> ibo;
    VAO vao;

    //IBO<GL_LINES> ibo_outlines;
    //VAO vao_outlines;

    VAO vao_picking;


public:
    TrianglesRenderBatch(size_t max_num_triangles);

    TrianglesRenderBatch(TetrahedralMesh& mesh);

    // TrianglesRenderBatch(const TrianglesRenderBatch&) = delete;  // No copying
    // TrianglesRenderBatch& operator=(const TrianglesRenderBatch&) = delete;

    void initFromMesh(TetrahedralMesh& mesh);

    void render();

    void renderPicking(const glm::mat4x4& mvp, const uint batch_idx);

    inline void deleteBuffers()
    {
        vbo.deleteBuffer();
        ibo.deleteBuffer();
        vao.deleteBuffer();
        // ibo_outlines.deleteBuffer();
        // vao_outlines.deleteBuffer();
        vao_picking.deleteBuffer();
    }

    inline uint numFree() const {return free.size();}

    int add(const Triangle& t);

    void remove(uint idx);

    inline void remove(const std::vector<uint>& idxs) {for (uint idx : idxs) remove(idx);}
};

}

#endif // TRIANGLESRENDERBATCH_H
