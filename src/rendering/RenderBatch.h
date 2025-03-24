#ifndef RENDERBATCH_H
#define RENDERBATCH_H

#include <sys/types.h>
#include <set>
#include "GLBuffers.h"
#include "../utils/Typedefs.h"

namespace MV
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

struct Line
{
    using VBO = VBO<GL_FLOAT, float, 3, 4>; // position, color
    static constexpr GLenum MODE = GL_LINES;

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

/**
 * @brief The RenderBatch class
 * PRIMITIVE is one of Point, Line or Triangle.
 */
template<typename PRIMITIVE>
class RenderBatch
{
protected:
    std::set<uint> updated;
    std::set<uint> free;

    PRIMITIVE::VBO vbo;
    IBO<PRIMITIVE::MODE> ibo;
    VAO vao;
    VAO vao_picking;

public:
    inline uint num_free_elements() const {return free.size();}

    inline uint max_num_elements() const {return ibo.size() / ibo.NUM_INDICES_PER_ELEMENT();}

    inline uint num_elements() const {return max_num_elements() - num_free_elements();}

    inline int add(const PRIMITIVE& p)
    {
        // If the batch is full, return -1
        if (free.empty()) return -1;

        // Retrieve the first free index
        uint idx = *free.begin();
        free.erase(free.begin());

        // Add the index to the updated list
        updated.insert(idx);

        // Store the data in the vbo
        p.buffer(vbo, idx);

        // Return the index
        return idx;
    }

    inline void remove(uint idx)
    {
        assert(free.contains(idx));

        // Set Vertex Buffer to Zero
        uint n = ibo.NUM_INDICES_PER_ELEMENT();
        for (uint i = 0; i < n; ++i)
        {
            vbo.setZero(n*idx+i);
        }

        // Declare as updated and mark as free
        updated.insert(idx);
        free.insert(idx);

        assert(free.size() + num_elements() == max_num_elements());
    }

    inline void remove(const std::vector<uint>& idxs)
    {
        for (uint idx : idxs)
        {
            remove(idx);
        }
    }

    inline void removeAll(size_t new_max_num_elements)
    {
        updated.clear();
        free.clear();
        for (uint i = 0; i < new_max_num_elements; ++i)
        {
            free.insert(i);
            updated.insert(i);
        }
    }

    inline void deleteBuffers()
    {
        vbo.deleteBuffer();
        ibo.deleteBuffer();
        vao.deleteBuffer();
        vao_picking.deleteBuffer();
    }

    inline std::vector<uint> allElementIndices()
    {
        std::vector<uint> idxs;
        idxs.reserve(max_num_elements());
        for (uint idx = 0; idx < max_num_elements(); ++idx)
        {
            idxs.push_back(idx);
        }
        return idxs;
    }
};

}

#endif // RENDERBATCH_H
