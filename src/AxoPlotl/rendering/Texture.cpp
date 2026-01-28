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
    glActiveTexture(unit);
    glBindTexture(GL_TEXTURE_2D, ID);
}

} // namespace AxoPlotl
