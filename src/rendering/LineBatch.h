#pragma once

#include "../utils/Utils.h"
#include "../commons/shader.h"
#include "VertexAttributesLayout.h"
#include <unordered_set>

namespace MV
{

class LineBatch
{
public:
    LineBatch(uint maxNumLines);
    ~LineBatch();

    template <typename Data>
    bool addLine(const Data& v0, const Data& v1)
    {
        if (numLines+1 > maxNumLines) return false;
        if (!initialized) initialize();

        int i = numLines;
        setVertexData(2*i+0, v0);
        setVertexData(2*i+1, v1);
        numLines += 1;

        return true;
    }

    void render();

private:
    //const uint NUM_FLOATS = 3 + 4 + 3; // per vertex
    VAL343f val;
    uint numLines;
    uint maxNumLines;
    std::vector<float> vertices;
    GLuint vao, vbo, ibo;
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
