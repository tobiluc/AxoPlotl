#ifndef TEXTURE_H
#define TEXTURE_H

#include <OpenGL/gltypes.h>

namespace AxoPlotl
{
class Texture {
public:
    GLuint ID = 0;

    Texture();

    ~Texture();

    void generateFromFile(const char* file);

    void use(GLenum unit);
};
}

#endif
