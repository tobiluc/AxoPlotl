#pragma once

#include "../utils/Utils.h"
#include "../commons/Shader.h"
#include "VertexAttributesLayout.h"
#include <unordered_set>

namespace MV
{

    class TriangleBatch
    {
    public:
        TriangleBatch(uint maxNumTriangles);
        ~TriangleBatch();

        //void setShader(Shader& shader);

        template <typename Data>
        bool addTriangle(const Data& v0, const Data& v1, const Data& v2)
        {
            if (numTriangles+1 > maxNumTriangles) return false;
            if (!initialized) initialize();

            int i = numTriangles;
            setVertexData(3*i+0, v0);
            setVertexData(3*i+1, v1);
            setVertexData(3*i+2, v2);
            numTriangles += 1;

            return true;
        }

        void render();

    private:
        VAL343f val;
        uint numTriangles;
        uint maxNumTriangles;
        std::vector<float> vertices;
        GLuint vao, vbo, ibo;
        //Shader& shader;
        bool initialized = false;
        std::unordered_set<int> changed;

        void initialize();

        template <typename Data>
        inline void setVertexData(int i, const Data& data)
        {
            changed.insert(i);
            i *= val.totalSize();
            for (int j = 0; j < val.totalSize(); ++j) vertices[i+j] = data[j];
        }
    };
}
