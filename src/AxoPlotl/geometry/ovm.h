#pragma once

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

}
