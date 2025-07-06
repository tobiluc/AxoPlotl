#ifndef GLBUFFERS_H
#define GLBUFFERS_H

#include <glad/glad.h>
#include <cassert>
#include <cstddef>
#include <sys/types.h>
#include <vector>
#include <GLFW/glfw3.h>

namespace AxoPlotl
{

// Vertex Attributes Layout of a common type
template <GLenum GLT, typename T, size_t... S>
struct VAL
{
public:
    VAL(size_t offset_layout, size_t offset_bytes) : sizes{S...}
    {
        size_t n_attributes = sizes.size();
        offsets_bytes.resize(n_attributes);
        layouts.resize(n_attributes);
        offsets_bytes[0] = offset_bytes;
        layouts[0] = offset_layout;
        for (int i = 1; i < n_attributes; ++i)
        {
            offsets_bytes[i] = offsets_bytes[i-1] + sizeof(T)*sizes[i-1];
            layouts[i] = layouts[i-1] + 1;
        }
        size = 0;
        for (int i = 0; i < n_attributes; ++i) size += sizes[i];
    }

    VAL() : VAL(0, 0)
    {}

    ~VAL() {}
    inline size_t attrOffsetBytes(size_t i) const {return offsets_bytes[i];}
    inline size_t attrSize(size_t i) const {return sizes[i];}
    inline size_t attrSizeBytes(size_t i) const {return sizeof(T)*sizes[i];}
    inline size_t numAttrs() const {return sizes.size();}
    inline size_t totalSize() const {return size;}
    inline size_t totalSizeBytes() const {return sizeof(T)*size;}
    inline size_t typeSizeBytes() const {return sizeof(T);}
    inline GLenum type() const {return GLT;}
    inline GLuint attrLayout(size_t i) const {return layouts[i];}
private:
    std::vector<size_t> sizes;
    std::vector<size_t> offsets_bytes;
    std::vector<GLuint> layouts;
    size_t size;
};
using VAL343f = VAL<GL_FLOAT, float, 3, 4, 3>; // e.g. position, color, normal
using VAL34f = VAL<GL_FLOAT, float, 3, 4>; // e.g. position, color
using VAL1i = VAL<GL_INT, int, 1>;
using VAL3f = VAL<GL_FLOAT, float, 3>;
using VAL1b = VAL<GL_BOOL, bool, 1>;

// Vertex Buffer Object
template <GLenum GLT, typename T, size_t... S>
struct VBO
{
public:
    VBO() :
        val()
    {};

    VBO(size_t layout_offset, size_t byte_offset) : val(layout_offset, byte_offset)
    {}

    ~VBO()
    {
        //deleteBuffer();
    }

    // Generates a new vertex buffer for a given number of vertices.
    // Assumes the used attribute slots to be 0,1,2,...
    GLuint generateNew(size_t nVertices)
    {
        resize(nVertices);

        deleteBuffer();

        glGenBuffers(1, &id);
        bind();
        glBufferData(GL_ARRAY_BUFFER, nVertices * val.totalSizeBytes(), &data[0], GL_DYNAMIC_DRAW);

        defineAttributes();

        return id;
    }

    inline void defineAttributes()
    {
        for (int i = 0; i < val.numAttrs(); ++i)
        {
            glVertexAttribPointer(val.attrLayout(i), val.attrSize(i), val.type(), GL_FALSE, val.totalSizeBytes(), (void*)(val.attrOffsetBytes(i)));
            glEnableVertexAttribArray(val.attrLayout(i));
        }

    }

    inline void defineAttributes(const std::vector<int>& attribute_indices)
    {
        for (int i : attribute_indices)
        {
            glVertexAttribPointer(val.attrLayout(i), val.attrSize(i), val.type(), GL_FALSE, val.totalSizeBytes(), (void*)(val.attrOffsetBytes(i)));
            glEnableVertexAttribArray(val.attrLayout(i));
        }

    }

    inline void bind()
    {
        glBindBuffer(GL_ARRAY_BUFFER, id);
    }

    inline void bufferSubData(size_t fromVertex, size_t nVertices)
    {
        glBufferSubData(GL_ARRAY_BUFFER, fromVertex*val.totalSizeBytes(), nVertices*val.totalSizeBytes(), &data[fromVertex*val.totalSize()]);
    }

    inline void enableAttributes(const std::vector<int> attribute_indices)
    {
        for (int i : attribute_indices) glEnableVertexAttribArray(val.attrLayout(i));
    }

    inline void enableAttributes()
    {
        for (int i = 0; i < val.numAttrs(); ++i) glEnableVertexAttribArray(val.attrLayout(i));
    }

    inline void disableAttributes()
    {
        for (int i = 0; i < val.numAttrs(); ++i) glDisableVertexAttribArray(val.attrLayout(i));
    }

    inline void resize(size_t nVertices)
    {
        this->nVertices = nVertices;
        data.resize(val.totalSize()*nVertices);
    }

    // Number of vertices
    inline size_t size() const
    {
        return nVertices;
    }

    inline void deleteBuffer()
    {
        if (id) glDeleteBuffers(1, &id);
    }

    template <typename Data>
    inline void set(size_t index, const Data& d)
    {
        for (int k = 0; k < val.totalSize(); ++k)
        {
            data[index*val.totalSize()+k] = d[k];
        }
    }

    inline void setZero(uint index)
    {
        for (int k = 0; k < val.totalSize(); ++k)
        {
            data[index*val.totalSize()+k] = {0};
        }
    }

    inline T& operator[] (size_t index)
    {
        return data[index];
    }

    inline const T& operator[] (size_t index) const
    {
        return data[index];
    }

    inline const VAL<GLT, T, S...>& layout() const
    {
        return val;
    }

    inline GLuint ID() const
    {
        return id;
    }

private:
    unsigned int nVertices = 0;
    GLuint id = 0;
    VAL<GLT, T, S...> val;
    std::vector<T> data;
    //std::vector<int> attrSlots;
};

struct DrawElementsIndirectCommand
{
    GLuint count;       // Number of indices to draw (2 for lines)
    GLuint instanceCount; // Number of instances (1 per line)
    GLuint firstIndex;  // Offset into the index buffer
    GLint baseVertex;   // Base vertex (0 if unused)
    GLuint baseInstance; // Base instance (not needed here)
};

// Index Buffer Object
template <GLenum GLMode>
struct IBO
{
public:
    IBO()
    {}

    ~IBO()
    {
        //deleteBuffer();
    }

    inline constexpr uint NUM_INDICES_PER_ELEMENT() const
    {
        return (GLMode == GL_POINTS)? 1 : (GLMode == GL_LINES)? 2 : 3;
    }

    // Buffers the given indices array
    GLuint generateNew(const std::vector<GLuint>& indices)
    {
        deleteBuffer();
        nIndices = indices.size();

        glGenBuffers(1, &id);
        bind();
        bufferData(indices);

        // Create the Count Array for glDrawMultiElements
        // Vector of 1s for points, 2s for lines, 3s for triangles
        uint n = NUM_INDICES_PER_ELEMENT();
        count.resize(indices.size() / n);
        std::fill(count.begin(), count.end(), n);



        return id;
    }

    // Buffers the indices array 0,1,2,3,...,n-1
    void generateNew(size_t nIndices)
    {
        std::vector<GLuint> indices(nIndices);
        for (unsigned int i = 0; i < nIndices; ++i) {indices[i] = i;}
        generateNew(indices);
    }

    inline void bufferData(const std::vector<GLuint>& indices)
    {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, nIndices * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
    }

    inline void deleteBuffer()
    {
        if (id) glDeleteBuffers(1, &id);
    }

    inline void bind()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
    }

    // Draws all elements
    inline void drawAll()
    {
        bind();
        glDrawElements(GLMode, nIndices, GL_UNSIGNED_INT, NULL);
    }

    // Only draws the given subset of elements.
    // indices must contain the offsets in bytes of where to find the elemnts to draw
    inline void drawMultiElements(const std::vector<void*>& begin_bytes)
    {
        bind();
        size_t n_elements = begin_bytes.size();
        assert(count.size() >= n_elements);
        glMultiDrawElements(GLMode, &count[0], GL_UNSIGNED_INT, &begin_bytes[0], n_elements);
    }

    inline size_t size() const
    {
        return nIndices;
    }

    inline GLuint ID() const
    {
        return id;
    }

private:
    GLuint id = 0;
    unsigned int nIndices; // max number of indices

    std::vector<GLsizei> count; // Count vector for drawMultiElements
};

// Vertex Array Object
struct VAO
{
public:
    VAO() {}

    ~VAO()
    {
        //deleteBuffer();
    }

    inline GLuint ID() const
    {
        return id;
    }

    inline void deleteBuffer()
    {
        if (id) glDeleteVertexArrays(1, &id);
    }

    inline void bind()
    {
        assert(id);
        glBindVertexArray(id);
        assert(glIsVertexArray(id));
    }

    inline void unbind()
    {
        glBindVertexArray(0);
    }

    inline GLuint generateNew()
    {
        glGenVertexArrays(1, &id);
        assert(id);
        bind();
        return id;
    }

private:
    GLuint id = 0;
};

}

#endif // GLBUFFERS_H
