#ifndef CUBE_H
#define CUBE_H

#include <glad/glad.h>
#include "../utils/gl.h"

class Cube {
public:

    Cube();

    // destructor
    ~Cube();

    /// render mesh of the cube
    void draw(GLenum mode = GL_TRIANGLES);


private:

    /// generate cube vertices/triangles and OpenGL buffers
    void initialize();


private:

    /// indices of the triangle vertices
    unsigned int n_indices_ = 0;

    // vertex array object
    GLuint vao_ = 0;
    /// vertex buffer object
    GLuint vbo_ = 0;
    /// normals buffer object
    GLuint nbo_ = 0;
    /// texture coordinates buffer object
    GLuint tbo_ = 0;
    /// index buffer object
    GLuint ibo_ = 0;
};

#endif
