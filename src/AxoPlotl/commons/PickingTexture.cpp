#include "PickingTexture.h"
#include <iostream>

namespace AxPl
{

void PickingTexture::init(unsigned int width, unsigned int height)
{
    deleteBuffers();
    this->width = width;
    this->height = height;

    // Create the FBO
    glGenFramebuffers(1, &fboID);
    glBindFramebuffer(GL_FRAMEBUFFER, fboID);

    // Create the texture object for the primitive information buffer
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32UI, width, height, 0, GL_RGB_INTEGER, GL_UNSIGNED_INT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);

    // Create the texture object for the depth buffer
    glGenTextures(1, &depthTextureID);
    glBindTexture(GL_TEXTURE_2D, depthTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTextureID, 0);

    // Verify that the FBO is correct
    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (Status != GL_FRAMEBUFFER_COMPLETE) {
        printf("FB error, status: 0x%x\n", Status);
        exit(1);
    }

    // Restore the default framebuffer
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

PickingTexture::Pixel PickingTexture::readPixel(unsigned int x, unsigned int y)
{
    y = getHeight() - y - 1;
    if (x >= width || y >= height) return {0,0,0};

    glBindFramebuffer(GL_READ_FRAMEBUFFER, fboID);

    glReadBuffer(GL_COLOR_ATTACHMENT0);

    Pixel p;
    glReadPixels(x, y, 1, 1, GL_RGB_INTEGER, GL_UNSIGNED_INT, &p);

    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

    return p;
}

void PickingTexture::bind()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboID);
    glBindFramebuffer(GL_FRAMEBUFFER, fboID);
}

void PickingTexture::unbind()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

}
