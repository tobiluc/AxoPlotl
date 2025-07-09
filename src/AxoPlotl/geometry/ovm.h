#pragma once

#include "AxoPlotl/geometry/Octree.h"
#include <OpenVolumeMesh/Core/Handles.hh>
#include <OpenVolumeMesh/Core/BaseEntities.hh>
#include <OpenVolumeMesh/Core/Properties/PropertyPtr.hh>
#include <OpenVolumeMesh/FileManager/TypeNames.hh>
#include <OpenVolumeMesh/FileManager/FileManager.hh>
#include <OpenVolumeMesh/Geometry/VectorT.hh>
#include <OpenVolumeMesh/Mesh/TetrahedralGeometryKernel.hh>
#include <OpenVolumeMesh/Core/GeometryKernel.hh>
#include <OpenVolumeMesh/Mesh/HexahedralMeshTopologyKernel.hh>

namespace AxoPlotl
{

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

template<typename MeshT>
int mesh_n_boundary_faces(MeshT& mesh)
{
    int n = 0;
    for (auto f_it = mesh.f_iter(); f_it.is_valid(); ++f_it)
        if (mesh.is_boundary(*f_it))
            n++;
    return n;
}

template<typename MeshT>
AABB computeBoundingBox(const MeshT& mesh) {
    AABB bb;
    bb.x0 = bb.y0 = bb.z0 = +std::numeric_limits<double>::infinity();
    bb.x1 = bb.y1 = bb.z1 = -std::numeric_limits<double>::infinity();
    for (auto v_it = mesh.v_iter(); v_it.is_valid(); ++v_it) {
        const auto& p = mesh.vertex(*v_it);
        bb.x0 = std::min(bb.x0, p[0]);
        bb.x1 = std::max(bb.x1, p[0]);
        bb.y0 = std::min(bb.y0, p[1]);
        bb.y1 = std::max(bb.y1, p[1]);
        bb.z0 = std::min(bb.z0, p[2]);
        bb.z1 = std::max(bb.z1, p[2]);
    }
}

}
