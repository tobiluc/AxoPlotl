#include "cube.h"
#include <vector>
#include <math.h>

//=============================================================================


Cube::Cube() {}

//-----------------------------------------------------------------------------


Cube::~Cube()
{
    if (vbo_)  glDeleteBuffers(1, &vbo_);
    if (nbo_)  glDeleteBuffers(1, &nbo_);
    if (tbo_)  glDeleteBuffers(1, &tbo_);
    if (ibo_)  glDeleteBuffers(1, &ibo_);
    if (vao_)  glDeleteVertexArrays(1, &vao_);
}


//-----------------------------------------------------------------------------


void Cube::initialize() 
{
    const unsigned int n_vertices = 36;
    const unsigned int n_triangles = 12;

    std::vector<GLfloat> positions(3 * n_vertices);
    std::vector<GLfloat>   normals(3 * n_vertices);
    std::vector<GLfloat> texcoords(2 * n_vertices);
    std::vector<GLuint >   indices(3 * n_triangles);

    // generate vertices
    positions = {
        // Right
        1.f,  -1.f, 1.f,
        1.f,  -1.f, -1.f,
        1.f,  1.f,  -1.f,
        1.f,  1.f,  -1.f,
        1.f,  -1.f, 1.f,
        1.f,  1.f,  1.f,

        // Left
        -1.f, -1.f, -1.f,
        -1.f, -1.f, 1.f,
        -1.f, 1.f,  1.f,
        -1.f, 1.f,  1.f,
        -1.f, -1.f, -1.f,
        -1.f, 1.f,  -1.f,

        // Top
        -1.f, 1.f,  1.f,
        1.f,  1.f,  1.f,
        1.f,  1.f,  -1.f,
        1.f,  1.f,  -1.f,
        -1.f, 1.f,  1.f,
        -1.f, 1.f,  -1.f,

        // Bottom
        -1.f, -1.f, 1.f,
        1.f,  -1.f, 1.f,
        1.f,  -1.f, -1.f,
        1.f,  -1.f, -1.f,
        -1.f, -1.f, 1.f,
        -1.f, -1.f, -1.f,

        // Front
        -1.f, -1.f, 1.f,
        1.f,  -1.f, 1.f,
        1.f,  1.f,  1.f,
        1.f,  1.f,  1.f,
        -1.f, -1.f, 1.f,
        -1.f, 1.f,  1.f,

        // Back
        1.f,  -1.f, -1.f,
        -1.f, -1.f, -1.f,
        -1.f, 1.f,  -1.f,
        -1.f, 1.f,  -1.f,
        1.f,  -1.f, -1.f,
        1.f,  1.f,  -1.f
    };

    normals = {
        // Right
        1,  0,  0,
        1,  0,  0,
        1,  0,  0,
        1,  0,  0,
        1,  0,  0,
        1,  0,  0,

        //Left
        -1, 0,  0,
        -1, 0,  0,
        -1, 0,  0,
        -1, 0,  0,
        -1, 0,  0,
        -1, 0,  0,

        // Top
        0,  1,  0,
        0,  1,  0,
        0,  1,  0,
        0,  1,  0,
        0,  1,  0,
        0,  1,  0,

        // Bottom
        0,  -1, 0,
        0,  -1, 0,
        0,  -1, 0,
        0,  -1, 0,
        0,  -1, 0,
        0,  -1, 0,

        // Front
        0,  0,  1,
        0,  0,  1,
        0,  0,  1,
        0,  0,  1,
        0,  0,  1,
        0,  0,  1,

        // Back
        0,  0,  -1,
        0,  0,  -1,
        0,  0,  -1,
        0,  0,  -1,
        0,  0,  -1,
        0,  0,  -1
    };

    texcoords = {
        // Right
        0,  0,
        1,  0,
        1,  1,
        1,  1,
        0,  0,
        0,  1,

        // Left
        0,  0,
        1,  0,
        1,  1,
        1,  1,
        0,  0,
        0,  1,

        // Top
        0,  0,
        1,  0,
        1,  1,
        1,  1,
        0,  0,
        0,  1,

        // Bottom
        0,  0,
        1,  0,
        1,  1,
        1,  1,
        0,  0,
        0,  1,

        // Front
        0,  0,
        1,  0,
        1,  1,
        1,  1,
        0,  0,
        0,  1,

        // Back
        0,  0,
        1,  0,
        1,  1,
        1,  1,
        0,  0,
        0,  1
    };

    indices = {
        0,  1,  2,
        3,  4,  5,

        6,  7,  8,
        9,  10, 11,

        12, 13, 14,
        15, 16, 17,

        18, 19, 20,
        21, 22, 23,

        24, 25, 26,
        27, 28, 29,

        30, 31, 32,
        33, 34, 35
    };
    n_indices_ = 3 * n_triangles;


    // generate vertex array object
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);


    // vertex positions -> attribute 0
    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, 3 * n_vertices * sizeof(float), &positions[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // normal vectors -> attribute 1
    glGenBuffers(1, &nbo_);
    glBindBuffer(GL_ARRAY_BUFFER, nbo_);
    glBufferData(GL_ARRAY_BUFFER, 3 * n_vertices * sizeof(float), &normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // texture coordinates -> attribute 2
    glGenBuffers(1, &tbo_);
    glBindBuffer(GL_ARRAY_BUFFER, tbo_);
    glBufferData(GL_ARRAY_BUFFER, 2 * n_vertices * sizeof(float), &texcoords[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    // triangle indices
    glGenBuffers(1, &ibo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * n_triangles * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
}


//-----------------------------------------------------------------------------


void Cube::draw(GLenum mode)
{
    if (n_indices_ == 0) initialize();

    glBindVertexArray(vao_);
    glDrawElements(mode, n_indices_, GL_UNSIGNED_INT, NULL);
    glBindVertexArray(0);
}


//=============================================================================
