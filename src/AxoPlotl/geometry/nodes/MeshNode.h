#pragma once

#include "GeometryNode.h"

namespace AxoPlotl
{

class MeshNode : public GeometryNode
{
private:
    PolyhedralMesh mesh_;

public:
    MeshNode(const PolyhedralMesh& _mesh, const std::string& _name="New Mesh", const glm::mat4& _transform = glm::mat4(1.0)) :
        GeometryNode("Mesh", _name, _transform), mesh_(_mesh)
    {
    }

    void addToRenderer(Scene* scene) override;

    void renderUIBody(Scene* scene) override;

    inline const PolyhedralMesh& mesh() const {return mesh_;}
};

}
