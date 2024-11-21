#include "texture.h"

Texture::Texture() : ID(0) {}

Texture::~Texture() {
    if (ID) glDeleteTextures(1, &ID);
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

void Texture::generateProcedural(int width, int height) {

    std::chrono::steady_clock::time_point time_start = std::chrono::steady_clock::now();

    // texture
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);

    // parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    std::vector<unsigned char> data;
    data.resize(width * height * 4);

    const float R = 2.f, r = 1.f;
    float tx, ty, tz;
    float phi, theta;

    SimplexNoise noise = SimplexNoise();

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            int i = (y * width + x) * 4;

            //MARBLE
            tx = x*0.005f;
            ty = y*0.005f;
            auto t = (2.f + noise.fractal(4, tx + 4.f * noise.fractal(4, tx, ty), ty + 4.f * noise.fractal(4, tx + 1.7f, ty + 4.6f))) * 0.25f;
            data[i + 0] = 255 * ((1-t)*1.f + t*0.48f); // Red
            data[i + 1] = 255 * ((1-t)*1.f + t*0.29f); // Green
            data[i + 2] = 255 * ((1-t)*1.f + t*0.00f); // Blue
            data[i + 3] = 255; // Alpha
        }
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    data = std::vector<unsigned char>();

    std::chrono::steady_clock::time_point time_end = std::chrono::steady_clock::now();
    std::cout << "Generated Texture (" << std::chrono::duration_cast<std::chrono::milliseconds> (time_end - time_start).count() << "[ms])" << std::endl;
}

void Texture::use(GLenum unit) {
    glActiveTexture(unit);
    glBindTexture(GL_TEXTURE_2D, ID);
}
