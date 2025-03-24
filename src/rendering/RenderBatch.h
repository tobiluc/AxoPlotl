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
    using VBO = VBO<GL_FLOAT, float, 3, 3>; // position, color
    static constexpr GLenum MODE = GL_POINTS;

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

struct Line
{
    using VBO = VBO<GL_FLOAT, float, 3, 3>; // position, color
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
};

struct Triangle
{
    using VBO = VBO<GL_FLOAT, float, 3, 3, 3, 1>; // position, color, normal, index
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

    std::vector<void*> begin_offsets_bytes; // begin offsets in bytes of active elements
    std::vector<GLuint> index_in_begin_offsets; // for each element, stores the index within begin_indices

    PRIMITIVE::VBO vbo;
    IBO<PRIMITIVE::MODE> ibo;
    VAO vao;
    VAO vao_picking;

public:
    inline uint num_free_elements() const {return free.size();}

    inline uint max_num_elements() const {return index_in_begin_offsets.size();}

    inline uint num_visible_elements() const {return begin_offsets_bytes.size();}

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

        // Update the array of active indices
        begin_offsets_bytes.push_back((void*)(ibo.NUM_INDICES_PER_ELEMENT() * idx * sizeof(GLuint)));
        index_in_begin_offsets[idx] = num_visible_elements()-1;

        // Store the data in the vbo
        p.buffer(vbo, idx);

        // Return the index
        return idx;
    }

    inline void remove(uint idx)
    {
        // Remove index from list of active indices
        begin_offsets_bytes.erase(begin_offsets_bytes.begin() + index_in_begin_offsets[idx]);

        // Shift indices of later points
        for (uint idx2 = idx; idx2 < index_in_begin_offsets.size(); ++idx2)
        {
            index_in_begin_offsets[idx2] -= 1;
        }

        // Declare as updated and mark as free
        updated.insert(idx);
        free.insert(idx);

        assert(free.size() + num_elements() == max_num_elements());
    }

    inline void removeAll(size_t new_max_num_elements)
    {
        updated.clear();
        free.clear();
        begin_offsets_bytes.clear();
        begin_offsets_bytes.reserve(new_max_num_elements);
        index_in_begin_offsets.resize(new_max_num_elements);
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
};

}

#endif // RENDERBATCH_H
