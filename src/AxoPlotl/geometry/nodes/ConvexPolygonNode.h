#pragma once

#include "AxoPlotl/geometry/nodes/GeometryNode.h"

namespace AxoPlotl
{

class ConvexPolygonNode : public GeometryNode
{
private:
    std::vector<Vec3f> vertices_;
    bool fill_ = true;

public:
    ConvexPolygonNode(const std::vector<Vec3f>& _vertices, const std::string& _name="New Polygon", const glm::mat4& _transform = glm::mat4(1.0)) :
        GeometryNode("Polygon", _name, _transform), vertices_(_vertices)
    {
    }

    void initRenderer(Scene* scene) override;

    void renderUIBody(Scene* scene) override;
};


}
