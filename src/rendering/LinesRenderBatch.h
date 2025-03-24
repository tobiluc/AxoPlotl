#ifndef LINESRENDERBATCH_H
#define LINESRENDERBATCH_H

#include "GLBuffers.h"
#include "PointsRenderBatch.h"

namespace MV
{

struct Line
{
    using VBO = VBO<GL_FLOAT, float, 3, 3>; // position, color

    std::array<Point, 2> vertices;

    Line(const Point& v0, const Point& v1) :
        vertices{v0, v1}
    {}

    inline void buffer(VBO& vbo, uint idx) const
    {
        for (unsigned char i = 0; i < 2; ++i)
        {
            vbo.set(2*idx+i, vertices[i].data());
        }
    }
};

class LinesRenderBatch
{
private:
    std::unordered_set<uint> updated;
    std::unordered_set<uint> free;

    Line::VBO vbo; // position, color
    IBO<GL_LINES> ibo;
    IBO<GL_LINES> ibo_features;
    VAO vao;

    //VAO vao_picking;

public:
    LinesRenderBatch(size_t max_num_lines);

    LinesRenderBatch(TetrahedralMesh& mesh);

    void initFromMesh(TetrahedralMesh& mesh);

    void render();

    inline void deleteBuffers()
    {
        vbo.deleteBuffer();
        ibo.deleteBuffer();
        ibo_features.deleteBuffer();
        vao.deleteBuffer();
    }

    inline uint numFree() const {return free.size();}

    int add(const Line& l);

    void remove(uint idx);

    inline void remove(const std::vector<uint>& idxs) {for (uint idx : idxs) remove(idx);}

};

}

#endif // LINESRENDERBATCH_H
