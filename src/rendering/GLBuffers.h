#ifndef GLBUFFERS_H
#define GLBUFFERS_H

#include <cstddef>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace MV
{

// Vertex Attributes Layout of a common type
template <GLenum GLT, typename T, size_t... S>
struct VAL
{
public:
    VAL() : sizes{S...}
    {
        offsets.resize(sizes.size());
        for (int i = 1; i < offsets.size(); ++i) offsets[i] = offsets[i-1] + sizes[i-1];
        size = offsets[offsets.size()-1] + sizes[offsets.size()-1];
    }

    ~VAL() {}
    inline size_t attrOffset(size_t i) const {return offsets[i];}
    inline size_t attrOffsetBytes(size_t i) const {return sizeof(T)*offsets[i];}
    inline size_t attrSize(size_t i) const {return sizes[i];}
    inline size_t attrSizeBytes(size_t i) const {return sizeof(T)*sizes[i];}
    inline size_t numAttrs() const {return sizes.size();}
    inline size_t totalSize() const {return size;}
    inline size_t totalSizeBytes() const {return sizeof(T) * size;}
    inline size_t typeSizeBytes() const {return sizeof(T);}
    inline GLenum type() const {return GLT;}
private:
    std::vector<size_t> sizes;
    std::vector<size_t> offsets;
    size_t size;
};
using VAL343f = VAL<GL_FLOAT, float, 3, 4, 3>; // e.g. position, color, normal
using VAL3f = VAL<GL_FLOAT, float, 3, 4>; // e.g. position, color

// Vertex Buffer Object
template <GLenum GLT, typename T, size_t... S>
struct VBO
{
public:
    VBO() :
        val(),
        attrSlots(val.numAttrs())
    {};

    ~VBO()
    {
        deleteBuffer();
    }

    // Generates a new vertex buffer for a given number of vertices and defines
    // which attribute slots should be used
    GLuint generateNew(size_t nVertices, const std::vector<int> attrSlots)
    {
        resize(nVertices);

        deleteBuffer();
        this->attrSlots = attrSlots;
        assert(this->attrSlots.size() == val.numAttrs());

        glGenBuffers(1, &id);
        bind();
        glBufferData(GL_ARRAY_BUFFER, nVertices * val.totalSizeBytes(), &data[0], GL_DYNAMIC_DRAW);

        // define attributes
        for (int i = 0; i < val.numAttrs(); ++i)
        {
            glVertexAttribPointer(attrSlots[i], val.attrSize(i), val.type(), GL_FALSE, val.totalSizeBytes(), (void*)(val.attrOffsetBytes(i)));
            glEnableVertexAttribArray(attrSlots[i]);
        }

        return id;
    }

    // Generates a new vertex buffer for a given number of vertices.
    // Assumes the used attribute slots to be 0,1,2,...
    GLuint generateNew(size_t nVertices)
    {
        std::vector<int> attrSlots(val.numAttrs());
        for (int i = 0; i < val.numAttrs(); ++i) attrSlots[i] = i;
        return generateNew(nVertices, attrSlots);
    }

    inline void bind()
    {
        glBindBuffer(GL_ARRAY_BUFFER, id);
    }

    inline void bufferSubData(size_t fromVertex, size_t nVertices)
    {
        glBufferSubData(GL_ARRAY_BUFFER, fromVertex*val.totalSizeBytes(), nVertices*val.totalSizeBytes(), &data[fromVertex*val.totalSize()]);
    }

    inline void enableAttributes()
    {
        for (int i = 0; i < val.numAttrs(); ++i) glEnableVertexAttribArray(attrSlots[i]);
    }

    inline void disableAttributes()
    {
        for (int i = 0; i < val.numAttrs(); ++i) glDisableVertexAttribArray(attrSlots[i]);
    }

    inline void resize(size_t nVertices)
    {
        this->nVertices = nVertices;
        data.resize(val.totalSize()*nVertices);
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

private:
    unsigned int nVertices = 0;
    GLuint id = 0;
    VAL<GLT, T, S...> val;
    std::vector<T> data;
    std::vector<int> attrSlots;
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
        deleteBuffer();
    }

    // Buffers the given indices array
    GLuint generateNew(const std::vector<GLuint>& indices)
    {
        deleteBuffer();
        nIndices = indices.size();

        glGenBuffers(1, &id);
        bind();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, nIndices * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

        return id;
    }

    // Buffers the indices array 0,1,2,3,...,n-1
    void generateNew(size_t nIndices)
    {
        std::vector<GLuint> indices(nIndices);
        for (int i = 0; i < nIndices; ++i) {indices[i] = i;}
        generateNew(indices);
    }

    inline void deleteBuffer()
    {
        if (id) glDeleteBuffers(1, &id);
    }

    inline void bind()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
    }

    inline void draw()
    {
        bind();
        glDrawElements(GLMode, nIndices, GL_UNSIGNED_INT, NULL);
    }

private:
    GLuint id = 0;
    unsigned int nIndices;
};

// Vertex Array Object
struct VAO
{
public:
    VAO() {}

    ~VAO()
    {
        deleteBuffer();
    }

    inline void deleteBuffer()
    {
        if (id) glDeleteVertexArrays(1, &id);
    }

    inline void bind()
    {
        glBindVertexArray(id);
    }

    inline void unbind()
    {
        glBindVertexArray(0);
    }

    inline GLuint generateNew()
    {
        glGenVertexArrays(1, &id);
        bind();
        return id;
    }

private:
    GLuint id = 0;
};

}

#endif // GLBUFFERS_H
