#include "TriangleBatch.h"

namespace MV
{
TriangleBatch::TriangleBatch(uint maxNumTriangles) :
        maxNumTriangles(maxNumTriangles),
        numTriangles(0),
        val(),
        vertices(3 * maxNumTriangles * val.totalSize())
    {
    }

    TriangleBatch::~TriangleBatch()
    {
        if (vbo) glDeleteBuffers(1, &vbo);
        if (ibo) glDeleteBuffers(1, &ibo);
        if (vao) glDeleteVertexArrays(1, &vao);
    }

    void TriangleBatch::initialize()
    {
        // generate vertex array object
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // compute the constant triangle indices (012,456,789,...)
        std::vector<uint> indices(3 * maxNumTriangles);
        for (int i = 0; i < maxNumTriangles; ++i)
        {
            indices[3*i+0] = 3*i+0;
            indices[3*i+1] = 3*i+1;
            indices[3*i+2] = 3*i+2;
        }

        // generate vertex buffer object
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, 3 * maxNumTriangles * val.totalSizeBytes(), &vertices[0], GL_DYNAMIC_DRAW);

        // generate index buffer object
        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * maxNumTriangles * sizeof(uint), &indices[0], GL_STATIC_DRAW);

        // define attributes
        for (int i = 0; i < val.numAttrs(); ++i)
        {
            glVertexAttribPointer(i, val.attrSize(i), val.type(), GL_FALSE, val.totalSizeBytes(), (void*)(val.attrOffsetBytes(i)));
            glEnableVertexAttribArray(i);
        }

        initialized = true;
    }

    void TriangleBatch::render()
    {
        if (!initialized) initialize();

        // only rebuffer part of data that has changed
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        while (!changed.empty())
        {
            int i = changed.extract(changed.begin()).value(); // pop
            glBufferSubData(GL_ARRAY_BUFFER, i*val.totalSizeBytes(), val.totalSizeBytes(), &vertices[i*val.totalSize()]);
        }

        glBindVertexArray(vao);
        for (int i = 0; i < val.numAttrs(); ++i) glEnableVertexAttribArray(i);

        glDrawElements(GL_TRIANGLES, 3 * numTriangles, GL_UNSIGNED_INT, NULL);
        
        for (int i = 0; i < val.numAttrs(); ++i) glDisableVertexAttribArray(i);
        glBindVertexArray(0);
    }
}
