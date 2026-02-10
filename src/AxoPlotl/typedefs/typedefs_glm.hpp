#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ostream>

namespace AxoPlotl
{

template <typename T>
using Vec2 = glm::vec<2, T>;
template <typename T>
using Vec3 = glm::vec<3, T>;
template <typename T>
using Vec4 = glm::vec<4, T>;
template <typename T>
using Mat3x3 = glm::mat<3, 3, T>;
template <typename T>
using Mat4x4 = glm::mat<4, 4, T>;

using Vec2i = Vec2<int>;
using Vec2f = Vec2<float>;
using Vec2d = Vec2<double>;
using Vec3i = Vec3<int>;
using Vec3f = Vec3<float>;
using Vec3d = Vec3<double>;
using Vec4f = Vec4<float>;
using Vec4d = Vec4<double>;
using Vec4i = Vec4<int>;
using Mat3x3f = Mat3x3<float>;
using Mat3x3d = Mat3x3<double>;
using Mat4x4f = Mat4x4<float>;
using Mat4x4d = Mat4x4<double>;

}

inline std::ostream& operator<<(std::ostream& os, const AxoPlotl::Vec3f& v) {
    return os << v.x << " " << v.y << " " << v.z;
}
