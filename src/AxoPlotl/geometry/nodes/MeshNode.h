#pragma once

#include "AxoPlotl/properties/property_calculations.hpp"
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
    std::vector<std::shared_ptr<PropertyFilterBase>> prop_filters_;
    int active_prop_filter_ = 0;

public:
    MeshNode(VolumeMesh& _mesh, const std::string& _name="New Mesh", const glm::mat4& _transform = glm::mat4(1.0)) :
        GeometryNode(_name, _transform), mesh_(std::move(_mesh))
    {
    }

    void init(Scene* scene) override;

    void renderUIBody(Scene* scene) override;

    void renderPickedUI(Scene* scene, const PickingTexture::Pixel& _picked) override;

    inline const VolumeMesh& mesh() const {return mesh_;}
};

}
