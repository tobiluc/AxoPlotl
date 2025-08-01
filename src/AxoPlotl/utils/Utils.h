#pragma once

#include "Typedefs.h"
#include <random>
#include <unistd.h>
#include <unordered_set>

namespace AxoPlotl
{
    // enum MeshRenderMode {NONE, CELLS, BOUNDARY_FACES, EDGES};

    // template <typename T, typename = void>
    // struct is_iterable : std::false_type {};

    // template <typename T>
    // struct is_iterable<T, std::void_t<decltype(std::begin(std::declval<T>())),
    //                                    decltype(std::end(std::declval<T>()))>> : std::true_type {};

    // template <typename T, typename = void>
    // struct has_subscript_operator : std::false_type {};

    // template <typename T>
    // struct has_subscript_operator<T, std::void_t<decltype(std::declval<T>()[std::declval<std::size_t>()])>> : std::true_type {};

    // // Recursive function to fill the array
    // template <typename T, std::size_t N, typename First, typename... Rest>
    // void fillArray(std::array<T, N>& arr, std::size_t& index, const First& first, const Rest&... rest)
    // {
    //     if constexpr (is_iterable<First>::value)
    //     {
    //         for (const auto& elem : first) arr[index++] = static_cast<T>(elem);
    //     }
    //     else if constexpr (has_subscript_operator<First>::value)
    //     {
    //         for (size_t i = 0; i < first.length(); ++i) arr[index++] = static_cast<T>(first[i]);
    //     }
    //     else
    //     {
    //         arr[index++] = static_cast<T>(first);
    //     }
    //     if constexpr (sizeof...(rest) > 0) {fillArray(arr, index, rest...);} // recursion
    // }

    // template <typename T, size_t N>
    // void fillArray(std::array<T, N>& arr, std::size_t& index) {}

    // // Flattens args into an array of type T ans size N
    // template <typename T, size_t N, typename... Args>
    // std::array<T, N> toArray(const Args&... args)
    // {
    //     static_assert(sizeof...(args) > 0);
    //     std::array<T, N> arr{};
    //     std::size_t index = 0;
    //     fillArray(arr, index, args...);
    //     if (index != N) {throw std::runtime_error("Number of elements does not match array size!");}
    //     return arr;
    // }

    template<typename Vec3T2, typename Vec3T1>
    inline Vec3T2 toVec3(const Vec3T1& v) {
        return Vec3T2{v[0], v[1], v[2]};
    }

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

    template <typename Vec3T>
    Vec3T tetIncenter(const Vec3T& A, const Vec3T& B, const Vec3T& C, const Vec3T& D)
    {
        const auto a = .5 * ((D - C).cross(D - B)).norm(); // Areas
        const auto b = .5 * ((D - C).cross(D - A)).norm();
        const auto c = .5 * ((D - A).cross(D - B)).norm();
        const auto d = .5 * ((C - B).cross(C - A)).norm();
        return (a*A + b*B + c*C + d*D) / (a + b + c + d);
    }

    template <typename Vec3T>
    void scaleTetrahedron(Vec3T& A, Vec3T& B, Vec3T& C, Vec3T& D, const double& delta)
    {
        if (delta <= 0) return;
        const auto I = tetIncenter(A,B,C,D);

        A += (1. - delta) * (I - A);
        B += (1. - delta) * (I - B);
        C += (1. - delta) * (I - C);
        D += (1. - delta) * (I - D);
    }

    template<typename Vec3T>
    inline Vec3T lerp3d(const Vec3T& a, const Vec3T& b, float t) {
        return a + (b - a) * t;
    }
    
} // namespace MV
