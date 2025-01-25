#ifndef PICKINGTEXTURE_H
#define PICKINGTEXTURE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace MV
{

class PickingTexture
{
public:
    PickingTexture() {}

    PickingTexture(unsigned int width, unsigned int height)
    {
        init(width, height);
    }

    ~PickingTexture()
    {
    }

    inline void deleteBuffers()
    {
        if (fboID) glDeleteFramebuffers(1, &fboID);
        if (textureID) glDeleteTextures(1, &textureID);
        if (depthTextureID) glDeleteTextures(1, &depthTextureID);
    }

    void bind();

    void unbind();

    struct PixelData
    {
        unsigned int object_id;
        unsigned int draw_id;
        unsigned int primitive_id;
    };

    PixelData readPixel(unsigned int x, unsigned int y);

private:
    GLuint fboID;
    GLuint textureID;
    GLuint depthTextureID;
    unsigned int width;
    unsigned int height;

    void init(unsigned int width, unsigned int height);
};



}

#endif // PICKINGTEXTURE_H
