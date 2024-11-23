#include "LineBatch.h"

namespace MV
{
LineBatch::LineBatch(uint maxNumLines) :
    val(),
    maxNumLines(maxNumLines),
    vertices(2 * maxNumLines * val.totalSize())
{

}

LineBatch::~LineBatch()
{
    if (vbo) glDeleteBuffers(1, &vbo);
    if (ibo) glDeleteBuffers(1, &ibo);
    if (vao) glDeleteVertexArrays(1, &vao);
}

void LineBatch::initialize()
{
    // generate vertex array object
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // compute the constant triangle indices (01,23,45,...)
    std::vector<uint> indices(2 * maxNumLines);
    for (int i = 0; i < maxNumLines; ++i)
    {
        indices[2*i+0] = 2*i+0;
        indices[2*i+1] = 2*i+1;
    }

    // generate vertex buffer object
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 2 * maxNumLines * val.totalSizeBytes(), &vertices[0], GL_DYNAMIC_DRAW);

    // generate index buffer object
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2 * maxNumLines * sizeof(uint), &indices[0], GL_STATIC_DRAW);

    // define attributes
    for (int i = 0; i < val.numAttrs(); ++i)
    {
        glVertexAttribPointer(i, val.attrSize(i), val.type(), GL_FALSE, val.totalSizeBytes(), (void*)(val.attrOffsetBytes(i)));
        glEnableVertexAttribArray(i);
    }

    // // vertex positions -> attribute 0
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), 0);
    // glEnableVertexAttribArray(0);

    // // vertex colors -> attribute 1
    // glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(3 * sizeof(float)));
    // glEnableVertexAttribArray(1);

    // // vertex normals -> attribute 2
    // glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(7 * sizeof(float)));
    // glEnableVertexAttribArray(2);

    initialized = true;
}

void LineBatch::render()
{
    if (!initialized) initialize();

    // only rebuffer part of data that has changed
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    //glBindBuffer(GL_ARRAY_BUFFER, vbo);
    while (!changed.empty())
    {
        int i = changed.extract(changed.begin()).value(); // pop
        glBufferSubData(GL_ARRAY_BUFFER, i*val.totalSizeBytes(), val.totalSizeBytes(), &vertices[i*val.totalSize()]);
    }

    glBindVertexArray(vao);
    for (int i = 0; i < val.numAttrs(); ++i) glEnableVertexAttribArray(i);

    glDrawElements(GL_LINES, 2 * numLines, GL_UNSIGNED_INT, NULL);

    for (int i = 0; i < val.numAttrs(); ++i) glDisableVertexAttribArray(i);
    glBindVertexArray(0);
}
}
