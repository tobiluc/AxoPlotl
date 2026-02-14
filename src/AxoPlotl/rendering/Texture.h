#ifndef TEXTURE_H
#define TEXTURE_H

#include "AxoPlotl/typedefs/typedefs_glm.hpp"
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

class ColorMap {
public:
    ColorMap() {}

    void create(const std::vector<float>& _data);

    void set_gradient(const std::vector<Vec3f>& _colors, int _N = 256);

    void set_gradient(const Vec3f& _a, const Vec3f& _b, int _N);

    void set_viridis(int N);

    void set_magma(int N);

    void set_inferno(int N);

    void set_plasma(int N);

    void set_rd_bu(int N);

    void set_coolwarm(int N);

    void update(const std::vector<float>& _data);

    void bind(GLenum uint);

    GLuint colormap_texture = 0;
};
}

#endif
