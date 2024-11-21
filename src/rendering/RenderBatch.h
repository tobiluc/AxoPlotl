#pragma once

#include "../utils/Utils.h"
#include "../commons/shader.h"

namespace MV
{
    struct VertexData
    {
        float x, y, z; // position
        float r, g, b, a; // color
        float nx, ny, nz; // normal
    };
    template <typename Vec3T1, typename Vec3T2>
    inline VertexData vertexData(const Vec3T1& pos, const Color::Color& color, const Vec3T2& normal)
    {
        return {(float)(pos[0]), (float)(pos[1]), (float)(pos[2]), color.r, color.g, color.b, color.a, (float)(normal[0]), (float)(normal[1]), (float)(normal[2])};
    }

    class RenderBatch
    {
    public:
        RenderBatch(uint maxNumTriangles, Shader& shader);
        ~RenderBatch();

        void initialize();

        bool addTriangle(const VertexData& v0, const VertexData& v1, const VertexData& v2);

        void render(GLenum mode = GL_TRIANGLES);

    private:
        uint numTriangles;
        uint maxNumTriangles;
        std::vector<float> vertices;
        GLuint vao, vbo, ibo;
        Shader& shader;
        bool initialized = false;

        inline void setVertexData(int i, const VertexData& data)
        {
            i *= (3 + 4 + 3);
            vertices[i+ 0] = data.x;
            vertices[i+ 1] = data.y;
            vertices[i+ 2] = data.z;
            vertices[i+ 3] = data.r;
            vertices[i+ 4] = data.g;
            vertices[i+ 5] = data.b;
            vertices[i+ 6] = data.a;
            vertices[i+ 7] = data.nx;
            vertices[i+ 8] = data.ny;
            vertices[i+ 9] = data.nz;
        }
    };
}
