#ifndef PICKINGTEXTURE_H
#define PICKINGTEXTURE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace AxoPlotl
{

/**
 * @brief Intended to access fragment data using a mouse click
 */
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

    struct Pixel
    {
        unsigned int object_index = 0;
        unsigned int primitive_index = 0;
        unsigned int primitive_type = 0;
    };

    Pixel readPixel(unsigned int x, unsigned int y);

    inline unsigned int getWidth() const {return width;}

    inline unsigned int getHeight() const {return height;}

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
