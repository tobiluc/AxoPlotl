#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "../utils/Typedefs.h"
#include "../utils/simplex_noise.h"

namespace MV
{
    class Triangle {
    public:

        Triangle();
        ~Triangle();

        /// render mesh of the triangle
        void draw(GLenum mode = GL_TRIANGLES);

        // Computes the model matrix used to represent a given triangle
        template <typename Vec3fT>
        inline void model_matrix(const Vec3fT& u,  const Vec3fT& v, const Vec3fT& w, Mat4x4f& A)
        {
            const auto d1 = v-u;
            const auto d2 = w-u;
            A[0] = Vec4f(d1[0], d1[1], d1[2], 0.0f);
            A[1] = Vec4f(d2[0], d2[1], d2[2], 0.0f);
            A[2] = Vec4f(0.0f, 0.0f, 1.0f, 0.0f);
            A[3] = Vec4f(u[0], u[1], u[2], 1.0f);
        }

    private:

        /// generate triangle vertices/triangle and OpenGL buffers
        void initialize();


    private:
        bool initialized = false;
        GLuint vao = 0; // vertex array object
        GLuint vbo = 0; // vertex buffer object
        GLuint ibo = 0;
    };
}
#endif
