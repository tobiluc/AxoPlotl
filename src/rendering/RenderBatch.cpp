#include "RenderBatch.h"

namespace MV
{
    RenderBatch::RenderBatch(uint maxNumTriangles, Shader& shader) :
        maxNumTriangles(maxNumTriangles),
        shader(shader)
    {

    }

    RenderBatch::~RenderBatch()
    {
        if (vbo) glDeleteBuffers(1, &vbo);
        if (ibo) glDeleteBuffers(1, &ibo);
        if (vao) glDeleteVertexArrays(1, &vao);
    }

    void RenderBatch::initialize()
    {
        std::cout << "Initializing Render Batch" << std::endl;

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
        glBufferData(GL_ARRAY_BUFFER, 3 * maxNumTriangles * sizeof(VertexData), vertices, GL_DYNAMIC_DRAW);

        // generate index buffer object
        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * maxNumTriangles * sizeof(uint), &indices[0], GL_STATIC_DRAW);

        // vertex positions -> attribute 0
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), 0);
        glEnableVertexAttribArray(0);

        // vertex colors -> attribute 1
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // vertex normals -> attribute 2
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(7 * sizeof(float)));
        glEnableVertexAttribArray(2);

        initialized = true;
        std::cout << "Initialized Render Batch" << std::endl;
    }

    bool RenderBatch::addTriangle(const VertexData& v0, const VertexData& v1, const VertexData& v2)
    {
        int i = numTriangles;
        setVertexData(3*i+0, v0);
        setVertexData(3*i+1, v1);
        setVertexData(3*i+2, v2);
        numTriangles += 1;

        return true;
    }

    void RenderBatch::render(GLenum mode)
    {
        if (!initialized) initialize();

        // TODO: only rebuffer part of data that has changed
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * maxNumTriangles * sizeof(VertexData), vertices);

        glBindVertexArray(vao);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        glDrawElements(mode, 3 * numTriangles, GL_UNSIGNED_INT, NULL);
        
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
        glBindVertexArray(0);
    }
}