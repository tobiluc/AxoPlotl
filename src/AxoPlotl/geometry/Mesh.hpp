#pragma once
#include "AxoPlotl/IO/OVMFileAccessor.h"
#include "OpenMesh/Core/IO/MeshIO.hh"
#include <AxoPlotl/geometry/nodes/GeometryNode.h>
#include <OpenVolumeMesh/Core/GeometryKernel.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <filesystem>

namespace AxoPlotl
{

class SurfaceMeshNode : public GeometryNode
{
public:
    using Mesh = OpenMesh::PolyMesh_ArrayKernelT<OpenMesh::DefaultTraits>;

    SurfaceMeshNode(const std::filesystem::path& _path) : GeometryNode("SurfaceMeshNode", _path.stem())
    {
        if (!OpenMesh::IO::read_mesh(mesh_, _path)) {
            std::cerr << "Cannot read surface mesh file " << _path << std::endl;
            deleted_ = true;
        }
    }

    SurfaceMeshNode(const Mesh&& _mesh) : GeometryNode("SurfaceMeshNode", "MeshName")
    {
        mesh_ = std::move(_mesh);
    }

    inline std::pair<glm::vec3, glm::vec3> getBBox() override {return bbox_;};

    void init(Scene* _scene) override;

    void renderUIBody(Scene* scene) override;

private:
    void init();
    std::pair<Vec3f,Vec3f> bbox_;
    Mesh mesh_;
    GL::MeshRenderer renderer_;
};

class VolumeMeshNode : public GeometryNode
{
public:
    using Mesh = OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f, OpenVolumeMesh::TopologyKernel>;

    VolumeMeshNode(const std::filesystem::path& _path) : GeometryNode("VolumeMeshNode", _path.stem())
    {
        if (!IO::readMesh(_path, mesh_)) {
            deleted_ = true;
        }
    }


    void init(Scene* _scene) override;

    void renderUIBody(Scene* scene) override;

    inline std::pair<glm::vec3, glm::vec3> getBBox() override {return bbox_;};

private:
    std::pair<Vec3f,Vec3f> bbox_;
    Mesh mesh_;
    GL::MeshRenderer renderer_;
};

}
