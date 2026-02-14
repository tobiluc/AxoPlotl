#include "Texture.h"
#include <chrono>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "AxoPlotl/commons/stb_image.h"

namespace AxoPlotl
{

Texture::Texture() : ID(0)
{}

Texture::~Texture() {
    if (ID) {
        glDeleteTextures(1, &ID);
        ID = 0;
    }
}

void Texture::generateFromFile(const char* file) {

    std::chrono::steady_clock::time_point time_start = std::chrono::steady_clock::now();

    // texture
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);

    // parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load and generate the texture
    int width, height, nrChannels;
    unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);
    if (data) {
        auto format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        std::chrono::steady_clock::time_point time_end = std::chrono::steady_clock::now();
        std::cout << "Loaded and generated texture from \"" << file << "\" ("
            << std::chrono::duration_cast<std::chrono::milliseconds> (time_end - time_start).count() << "[ms])" << std::endl;
    }
    else {
        std::cerr << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
}

void Texture::use(GLenum unit) {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, ID);
}

void ColorMap::create(const std::vector<float> &_data)
{
    if (colormap_texture == 0) {
        glGenTextures(1, &colormap_texture);
    }

    update(_data);

    glBindTexture(GL_TEXTURE_1D, colormap_texture);

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_1D, 0);
}

void ColorMap::set_gradient(const std::vector<Vec3f>& _colors, int N)
{
    std::vector<float> data;
    data.reserve(N * 4);

    for (int i = 0; i < N; ++i)
    {
        float t = float(i) / float(N - 1);

        float scaled = t * (_colors.size() - 1);
        int idx0 = std::min(int(std::floor(scaled)), int(_colors.size() - 2));
        int idx1 = idx0 + 1;
        float local_t = scaled - idx0;

        const Vec3f &c0 = _colors[idx0];
        const Vec3f &c1 = _colors[idx1];

        float r = c0.r + local_t * (c1.r - c0.r);
        float g = c0.g + local_t * (c1.g - c0.g);
        float b = c0.b + local_t * (c1.b - c0.b);

        data.push_back(r);
        data.push_back(g);
        data.push_back(b);
        data.push_back(1.0f);
    }

    update(data);
}

void ColorMap::set_gradient(const Vec3f& _a, const Vec3f& _b, int _N)
{
    set_gradient({_a, _b}, _N);
}

void ColorMap::set_viridis(int N)
{
    static const std::vector<Vec3f> viridis = {
        {0.267004f, 0.004874f, 0.329415f},
        {0.282327f, 0.140926f, 0.457517f},
        {0.253935f, 0.265254f, 0.529983f},
        {0.206756f, 0.371758f, 0.553117f},
        {0.163625f, 0.471133f, 0.558148f},
        {0.127568f, 0.566949f, 0.550556f},
        {0.134692f, 0.658636f, 0.517649f},
        {0.266941f, 0.748751f, 0.440573f},
        {0.477504f, 0.821444f, 0.318195f},
        {0.741388f, 0.873449f, 0.149561f},
        {0.993248f, 0.906157f, 0.143936f}
    };
    set_gradient(viridis, N);
}

void ColorMap::set_plasma(int N)
{
    static const std::vector<Vec3f> plasma = {
        {0.050383f, 0.029803f, 0.527975f},
        {0.336627f, 0.063956f, 0.769953f},
        {0.648925f, 0.133936f, 0.781554f},
        {0.863001f, 0.386567f, 0.764393f},
        {0.975324f, 0.639465f, 0.519905f},
        {0.993248f, 0.906157f, 0.143936f}
    };
    set_gradient(plasma, N);
}

void ColorMap::set_inferno(int N)
{
    static const std::vector<Vec3f> inferno = {
        {0.001462f, 0.000466f, 0.013866f},
        {0.200403f, 0.018246f, 0.337641f},
        {0.468160f, 0.140614f, 0.493067f},
        {0.772053f, 0.313162f, 0.392499f},
        {0.940015f, 0.631844f, 0.152819f},
        {0.987053f, 0.991438f, 0.749504f}
    };
    set_gradient(inferno, N);
}

void ColorMap::set_magma(int N)
{
    static const std::vector<Vec3f> magma = {
        {0.001462f, 0.000466f, 0.013866f},
        {0.267004f, 0.004874f, 0.329415f},
        {0.477504f, 0.064254f, 0.557964f},
        {0.741388f, 0.214372f, 0.497832f},
        {0.965960f, 0.618919f, 0.319167f},
        {0.987053f, 0.991438f, 0.749504f}
    };
    set_gradient(magma, N);
}

void ColorMap::set_rd_bu(int N)
{
    static const std::vector<Vec3f> rd_bu = {
        {0.403921f, 0.000000f, 0.121569f},
        {0.698039f, 0.094118f, 0.168627f},
        {0.870588f, 0.619608f, 0.592157f},
        {1.0f, 1.0f, 1.0f},
        {0.619608f, 0.792157f, 0.870588f},
        {0.098039f, 0.396078f, 0.686275f},
        {0.0f, 0.003922f, 0.258824f}
    };
    set_gradient(rd_bu, N);
}

void ColorMap::set_coolwarm(int N)
{
    static const std::vector<Vec3f> coolwarm = {
        {0.229805f, 0.298717f, 0.753683f}, // blue
        {0.541643f, 0.636480f, 0.864786f},
        {0.865003f, 0.865003f, 0.865003f}, // white / center
        {0.991248f, 0.697083f, 0.417556f},
        {0.956871f, 0.211055f, 0.131898f}  // red
    };
    set_gradient(coolwarm, N);
}

void ColorMap::update(const std::vector<float>& _data)
{
    glBindTexture(GL_TEXTURE_1D, colormap_texture);
    glTexImage1D(
        GL_TEXTURE_1D,
        0,
        GL_RGBA8,
        _data.size()/4,
        0,
        GL_RGBA,
        GL_FLOAT,
        _data.data()
    );
    glBindTexture(GL_TEXTURE_1D, 0);
}

void ColorMap::bind(GLenum unit) {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_1D, colormap_texture);
}

} // namespace AxoPlotl
