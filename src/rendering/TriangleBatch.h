#pragma once

#include "../utils/Utils.h"
#include "../commons/shader.h"

namespace MV
{

    class TriangleBatch
    {
    public:
        TriangleBatch(uint maxNumTriangles, Shader& shader);
        ~TriangleBatch();

        void initialize();

        bool addTriangle(const VertexData& v0, const VertexData& v1, const VertexData& v2);

        void render();

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
