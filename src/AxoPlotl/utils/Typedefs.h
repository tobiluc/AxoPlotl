#pragma once

#include "Color.h"
#define _USE_MATH_DEFINES
#include <math.h>

//#include "gl.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <OpenVolumeMesh/Core/Handles.hh>
#include <OpenVolumeMesh/Core/BaseEntities.hh>
#include <OpenVolumeMesh/Core/Properties/PropertyPtr.hh>
#include <OpenVolumeMesh/FileManager/TypeNames.hh>
#include <OpenVolumeMesh/FileManager/FileManager.hh>
#include <OpenVolumeMesh/Geometry/VectorT.hh>
#include <OpenVolumeMesh/Mesh/TetrahedralGeometryKernel.hh>
#include <OpenVolumeMesh/Core/GeometryKernel.hh>
#include <OpenVolumeMesh/Mesh/HexahedralMeshTopologyKernel.hh>

#include <nlohmann/json.hpp>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace AxoPlotl
{

using uint = unsigned int;
using uchar = unsigned char;

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
using Mat3x3f = Mat3x3<float>;
using Mat3x3d = Mat3x3<double>;
using Mat4x4f = Mat4x4<float>;
using Mat4x4d = Mat4x4<double>;

inline std::ostream& operator<<(std::ostream& os, const Vec3f& v) {
    return os << v.x << " " << v.y << " " << v.z;
}



namespace OVM = OpenVolumeMesh;
using VertexHandle = OVM::VertexHandle;
using EdgeHandle = OVM::EdgeHandle;
using HalfEdgeHandle = OVM::HalfEdgeHandle;
using FaceHandle = OVM::FaceHandle;
using HalfFaceHandle = OVM::HalfFaceHandle;
using CellHandle = OVM::CellHandle;
using TetrahedralMesh = OVM::TetrahedralGeometryKernel<OVM::Vec3d, OpenVolumeMesh::TetrahedralMeshTopologyKernel>;
using PolyhedralMesh = OVM::GeometryKernel<OVM::Vec3d, OpenVolumeMesh::TopologyKernel>;
using HexahedralMesh = OVM::GeometryKernel<OVM::Vec3d, OpenVolumeMesh::HexahedralMeshTopologyKernel>;

struct Light {Color ambient, diffuse, specular;};

namespace IO
{

template<typename T>
bool deserialize(const std::string& filename, T& obj) {
    std::ifstream file(filename);
    if (!file) {return false;}
    try {
        nlohmann::json j;
        file >> j;
        obj = j.get<T>();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

template<typename T>
bool serialize(const std::string& filename, const T& obj) {
    std::ofstream file(filename);
    if (!file) {return false;}

    try {
        file << nlohmann::json(obj).dump(4);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}
}


} // !namespace AxoPlotl

namespace nlohmann {

inline void to_json(json& j, const AxoPlotl::Vec3f& v) {
    j = json{{"x", v.x}, {"y", v.y}, {"z", v.z}};
}

inline void from_json(const json& j, AxoPlotl::Vec3f& v) {
    j.at("x").get_to(v.x);
    j.at("y").get_to(v.y);
    j.at("z").get_to(v.z);
}

} // !namespace nlohmann
