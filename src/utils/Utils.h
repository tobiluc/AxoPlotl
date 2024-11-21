#pragma once

#include "Typedefs.h"
#include <iostream>
#include <random>
#include <unistd.h>

namespace MV
{
enum MeshRenderMode {NONE, CELLS, FACES, BOUNDARY_FACES};

    class Random
    {
    public:
        Random()
        {
            std::random_device rd;
            gen = std::mt19937(rd());
            dist = std::uniform_real_distribution<float>(0.0, 1.0);
        }
        inline float randf() {return dist(gen);}
    private:
        std::mt19937 gen;
        std::uniform_real_distribution<float> dist;
    };

    namespace Color
    {
    struct Color {float r, g, b, a;};
    constexpr Color RED = {1, 0, 0, 1};
    constexpr Color GREEN = {0, 1, 0, 1};
    constexpr Color BLUE = {0, 0, 1, 1};
    constexpr Color WHITE = {1, 1, 1, 1};
    constexpr Color BLACK = {0, 0, 0, 1};
    }

    struct VertexData
    {
        float x, y, z; // position
        float r, g, b, a; // color
        float nx, ny, nz; // normal
    };
    template <typename Vec3T1, typename Vec3T2>
    inline VertexData vertexData(const Vec3T1& pos, const Color::Color& color, const Vec3T2& normal)
    {
        return {(float)(pos[0]), (float)(pos[1]), (float)(pos[2]), color.r, color.g, color.b, color.a, (float)(normal[0]), (float)(normal[1]), (float)(normal[2])};
    }

    template <typename Vec3T>
    void scaleTetrahedron(Vec3T& A, Vec3T& B, Vec3T& C, Vec3T& D, const double& delta)
    {
        if (delta <= 0) return;
        const auto a = .5 * ((D - C).cross(D - B)).norm(); // Areas
        const auto b = .5 * ((D - C).cross(D - A)).norm();
        const auto c = .5 * ((D - A).cross(D - B)).norm();
        const auto d = .5 * ((C - B).cross(C - A)).norm();
        const auto I = (a*A + b*B + c*C + d*D) / (a + b + c + d); // Incenter

        const auto VTimes6 = fabs((A - D).dot((B - D) % (C - D)));
        if (VTimes6 == 0) return; // Volume
        //const auto r = .5 * VTimes6 / (a + b + c + d); // Inradius
        A += (1. - delta) * (I - A);
        B += (1. - delta) * (I - B);
        C += (1. - delta) * (I - C);
        D += (1. - delta) * (I - D);
    }


    inline std::string toLower(const std::string& s)
    {
        std::string r = s;
        std::transform(r.begin(), r.end(), r.begin(),[](unsigned char c){return std::tolower(c);});
        return r;
    }

    inline std::pair<std::string,std::string> splitFilenameExtension(const std::string& filename)
    {
        auto i = filename.find_last_of(".");
        return std::make_pair(filename.substr(0,i), filename.substr(i));
    }

    inline std::string getWorkingDirectory()
    {
        const size_t size = 1024;
        char buffer[size];
        if (getcwd(buffer, size) != NULL) return buffer;
        return "ERROR";
    }

    int mesh_n_boundary_faces(TetrahedralMesh& mesh);
    
} // namespace MV
