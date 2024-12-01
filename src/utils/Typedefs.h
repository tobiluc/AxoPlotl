#pragma once

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

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace MV
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

using Color = Vec3f;
struct Light {Color ambient, diffuse, specular;};

struct MeshRenderSettings
{
public:
    MeshRenderSettings() :
        showCells(true),
        showFaces(true),
        showEdges(true),
        showVertices(true),
        light({Color(0.7,0.7,0.7), Color(0.2,0.2,0.2), Color(0.1,0.1,0.1)}),
        cellScale(0.9f),
        outlineWidth(2.0f),
        lineWidth(2.0f),
        pointSize(5.0f),
        outlineColor(Color(0,0,0)),
        useColorOverride(false),
        colorOverride(Color(1,1,1))
    {

    }

    bool showCells, showFaces, showEdges, showVertices;
    Light light;
    float cellScale;
    float outlineWidth;
    float pointSize;
    float lineWidth;
    Color outlineColor;
    bool useColorOverride = false;
    Color colorOverride;
};
}
