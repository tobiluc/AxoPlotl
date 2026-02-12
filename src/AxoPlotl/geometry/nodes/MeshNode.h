#pragma once

#include "AxoPlotl/properties/property_filters.hpp"
#include "AxoPlotl/typedefs/typedefs_OpenVolumeMesh.hpp"
#include "GeometryNode.h"

namespace AxoPlotl
{

class MeshNode : public GeometryNode
{
private:
    VolumeMesh mesh_;
    std::optional<OpenVolumeMesh::PropertyStorageBase*> prop_;
    std::shared_ptr<PropertyFilterBase> prop_filter;

public:
    MeshNode(const VolumeMesh& _mesh, const std::string& _name="New Mesh", const glm::mat4& _transform = glm::mat4(1.0)) :
        GeometryNode(_name, _transform), mesh_(_mesh)
    {

    }

    void init(Scene* scene) override;

    void renderUIBody(Scene* scene) override;

    inline const VolumeMesh& mesh() const {return mesh_;}
};

}
