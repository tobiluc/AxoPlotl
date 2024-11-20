#include "Triangle.h"

namespace MV
{
    Triangle::Triangle()
    {
    }

    Triangle::~Triangle()
    {
        if (vbo) glDeleteBuffers(1, &vbo);
        if (ibo)  glDeleteBuffers(1, &ibo);
        if (vao) glDeleteVertexArrays(1, &vao);
    }

    void Triangle::initialize()
    {
        std::vector<GLfloat> positions = {
            0.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f
        };
        std::vector<GLuint > indices = {0, 1, 2};

        // generate vertex array object
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // vertex positions -> attribute 0
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), &positions[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        // triangle indices
        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * 1 * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

        initialized = true;
    }

    void Triangle::draw(GLenum mode)
    {
        if (!initialized) initialize();
        glBindVertexArray(vao);
        glDrawElements(mode, 3, GL_UNSIGNED_INT, NULL);
        glBindVertexArray(0);
    }
}