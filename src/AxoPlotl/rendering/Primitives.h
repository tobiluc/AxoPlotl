#pragma once

#include "AxoPlotl/rendering/GLBuffers.h"
#include "AxoPlotl/utils/Typedefs.h"

namespace AxoPlotl::Rendering
{

//===============
// Primitives

struct Point
{
    using VBO = VBO<GL_FLOAT, float, 3, 4>; // position, color
    static constexpr GLenum MODE = GL_POINTS;

    std::array<float, 3> position;
    std::array<float, 4> color;

    template<typename Position, typename Color>
    Point(const Position& pos, const Color& col) :
        position({(float)pos[0], (float)pos[1], (float)pos[2]}),
        color({(float)col[0], (float)col[1], (float)col[2], 1.0f})
    {}

    inline const std::array<float,7> data() const
    {
        return {
            position[0],position[1],position[2],
            color[0],color[1],color[2],color[3]
        };
    }

    inline void buffer(VBO& vbo, uint idx) const
    {
        vbo.set(idx, data());
    }

    inline void clearBuffer(VBO& vbo, uint idx) const
    {
        vbo.setZero(idx);
    }
};

inline std::ostream& operator<<(std::ostream& os, const Point& p) {
    return os << "Point(" << p.position[0] << "," <<
           p.position[1] << "," << p.position[2] << ")";
}

struct Line
{
    using VBO = VBO<GL_FLOAT, float, 3, 4>; // position, color
    static constexpr GLenum MODE = GL_LINES;

    std::array<Point, 2> vertices;

    Line(const Point& v0, const Point& v1) :
        vertices{v0, v1}
    {}

    Line(const Vec3f& p0, const Vec3f& p1, Color color) :
        Line(Point(p0,color),Point(p1,color))
    {}

    inline void buffer(VBO& vbo, uint idx) const
    {
        for (unsigned char i = 0; i < 2; ++i)
        {
            vbo.set(2*idx+i, vertices[i].data());
        }
    }

    inline void clearBuffer(VBO& vbo, uint idx) const
    {
        vbo.setZero(2*idx+0);
        vbo.setZero(2*idx+1);
    }
};

struct Triangle
{
    using VBO = VBO<GL_FLOAT, float, 3, 4, 3, 1>; // position, color, normal, index
    static constexpr GLenum MODE = GL_TRIANGLES;

    std::array<Point, 3> vertices;
    std::array<float, 3> normal;

    Triangle(const Point& p0, const Point& p1, const Point& p2) :
        vertices{p0, p1, p2}
    {
        computeNormal();
    }

    Triangle(const Vec3f& p0, const Vec3f& p1, const Vec3f& p2, const Color& color) :
        Triangle(Point(p0, color), Point(p1, color), Point(p2, color))
    {}

    inline void buffer(VBO& vbo, uint idx) const
    {
        for (unsigned char i = 0; i < 3; ++i)
        {
            std::array<float, 11> data = {
                vertices[i].position[0],vertices[i].position[1],vertices[i].position[2],
                vertices[i].color[0],vertices[i].color[1],vertices[i].color[2],vertices[i].color[3],
                normal[0], normal[1], normal[2],
                (float)(idx+1)
            };
            vbo.set(3*idx+i, data);
        }
    }

    inline void clearBuffer(VBO& vbo, uint idx) const
    {
        vbo.setZero(3*idx+0);
        vbo.setZero(3*idx+1);
        vbo.setZero(3*idx+2);
    }

private:
    void computeNormal();
};

// !Primitives
//===============

}
