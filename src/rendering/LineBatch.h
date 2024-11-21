#pragma once

#include "../utils/Utils.h"
#include "../commons/shader.h"

namespace MV
{

class LineBatch
{
public:
    LineBatch(uint maxNumLines, Shader& shader);
    ~LineBatch();

    void initialize();

    bool addLine(const Vec3f& p, const Vec3f& q, const Color::Color& col);

    void render();

private:
    uint numLines;
    uint maxNumLines;
    std::vector<float> vertices;
    GLuint vao, vbo, ibo;
    Shader& shader;
    bool initialized = false;
};
}
