#pragma once

#include "AxoPlotl/properties/property_filters.hpp"
#include "GeometryNode.h"

namespace AxoPlotl
{

class MeshNode : public GeometryNode
{
private:
    std::pair<Vec3f,Vec3f> bbox_;
    PolyhedralMesh mesh_;
    std::optional<OpenVolumeMesh::PropertyStorageBase*> prop_;
    std::shared_ptr<PropertyFilterBase> prop_filter;

public:
    MeshNode(const PolyhedralMesh& _mesh, const std::string& _name="New Mesh", const glm::mat4& _transform = glm::mat4(1.0)) :
        GeometryNode("Mesh", _name, _transform), mesh_(_mesh)
    {

    }

    void initRenderer(Scene* scene) override;

    void renderUIBody(Scene* scene) override;

    inline std::pair<Vec3f,Vec3f> getBBox() override {
        return bbox_;
    }

    inline const PolyhedralMesh& mesh() const {return mesh_;}
};

}
