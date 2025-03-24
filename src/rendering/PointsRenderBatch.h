#ifndef POINTSRENDERBATCH_H
#define POINTSRENDERBATCH_H

#include "GLBuffers.h"
#include <array>
#include <sys/types.h>
#include <unordered_set>
#include "../utils/Typedefs.h"

namespace MV
{

struct Point
{
    using VBO = VBO<GL_FLOAT, float, 3, 3>; // position, color

    std::array<float, 3> position;
    std::array<float, 3> color;

    template<typename Position, typename Color>
    Point(const Position& pos, const Color& col) :
        position({(float)pos[0], (float)pos[1], (float)pos[2]}),
        color({(float)col[0], (float)col[1], (float)col[2]})
    {}

    inline const std::array<float,6> data() const
    {
        return {
            position[0],position[1],position[2],
            color[0],color[1],color[2]
        };
    }

    inline void buffer(VBO& vbo, uint idx) const
    {
        vbo.set(idx, data());
    }
};

class PointsRenderBatch
{
private:
    std::unordered_set<uint> updated;
    std::unordered_set<uint> free;

    Point::VBO vbo; // position, color
    IBO<GL_POINTS> ibo;
    VAO vao;

    //VAO vao_picking;



public:
    PointsRenderBatch(size_t max_num_vertices);

    PointsRenderBatch(TetrahedralMesh& mesh);

    void initFromMesh(TetrahedralMesh& mesh);

    void render();

    inline void deleteBuffers()
    {
        vbo.deleteBuffer();
        ibo.deleteBuffer();
        vao.deleteBuffer();
    }

    inline uint numFree() const {return free.size();}

    int add(const Point& p);

    void remove(uint idx);

    inline void remove(const std::vector<uint>& idxs) {for (uint idx : idxs) remove(idx);}
};

}

#endif // POINTSRENDERBATCH_H
