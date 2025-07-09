#ifndef RENDERBATCH_H
#define RENDERBATCH_H

//#include <sys/types.h>
#include <set>
#include "GLBuffers.h"
//#include "../utils/Typedefs.h"

namespace AxoPlotl::Rendering
{

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
    //IBO<PRIMITIVE::MODE> ibo_visible;

    VAO vao;
    VAO vao_picking;

public:
    inline uint num_free_elements() const {return free.size();}

    inline uint max_num_elements() const {return ibo.size() / ibo.NUM_INDICES_PER_ELEMENT();}

    inline uint num_elements() const {return max_num_elements() - num_free_elements();}

    // Sets the i-th primitive (in)visible
    inline void setVisible(const uint idx, bool visible)
    {
        //TODO
    }

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

    inline std::vector<uint> add(const std::vector<PRIMITIVE>& ps)
    {
        if (num_free_elements() < ps.size()) return {};

        std::vector<uint> idxs;
        idxs.reserve(ps.size());
        for (const auto& p : ps)
        {
            idxs.push_back(add(p));
        }
        return idxs;
    }

    // inline void remove(uint idx)
    // {
    //     assert(free.contains(idx));

    //     // Set Vertex Buffer to Zero
    //     uint n = ibo.NUM_INDICES_PER_ELEMENT();
    //     for (uint i = 0; i < n; ++i)
    //     {
    //         vbo.setZero(n*idx+i);
    //     }

    //     // Declare as updated and mark as free
    //     updated.insert(idx);
    //     free.insert(idx);

    //     assert(free.size() + num_elements() == max_num_elements());
    // }

    // inline void remove(const std::vector<uint>& idxs)
    // {
    //     for (uint idx : idxs)
    //     {
    //         remove(idx);
    //     }
    // }

    /// Mark all as free, set vertex buffer to zero
    inline void clearVertexBuffer()
    {
        updated.clear();
        free.clear();

        // Set Vertex Buffer to Zero
        vbo.setZero();

        // Mark all as free
        for (uint i = 0; i < max_num_elements(); ++i)
        {
            free.insert(i);
            updated.insert(i);
        }
    }

    /// Regeneate buffers and resize
    inline void regenerateBuffers(size_t num_elements)
    {
        vao.generateNew();
        vbo.generateNew(ibo.NUM_INDICES_PER_ELEMENT() * num_elements);
        ibo.generateNew(ibo.NUM_INDICES_PER_ELEMENT() * num_elements);

        clearVertexBuffer();

        vao.unbind();

        // Generate picking buffers
        vao_picking.generateNew();
        vbo.defineAttributes({0,3});
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
