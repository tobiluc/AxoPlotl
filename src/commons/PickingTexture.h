#ifndef PICKINGTEXTURE_H
#define PICKINGTEXTURE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace MV
{

class PickingTexture
{
public:
    PickingTexture(unsigned int width, unsigned int height) :
        WIDTH(width),
        HEIGHT(height)
    {
        init();
    }

    ~PickingTexture()
    {
        if (fboID) glDeleteFramebuffers(1, &fboID);
        if (textureID) glDeleteTextures(1, &textureID);
        if (depthTextureID) glDeleteTextures(1, &depthTextureID);
    }

    void bind();

    void unbind();

    struct PixelData
    {
    };

private:
    GLuint fboID;
    GLuint textureID;
    GLuint depthTextureID;

    void init();

    const unsigned int WIDTH, HEIGHT;
};



}

#endif // PICKINGTEXTURE_H
