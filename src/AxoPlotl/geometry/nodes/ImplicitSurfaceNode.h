#pragma once

#include "AxoPlotl/geometry/Surface.h"
#include "AxoPlotl/geometry/nodes/GeometryNode.h"

namespace AxoPlotl
{

class ImplicitSurfaceNode : public GeometryNode
{
private:
    ImplicitSurfaceFunction f_;
    Color color_;
    int octree_depth_ = 3;
    char input_buffer_[1024];

public:
    ImplicitSurfaceNode(const std::string& _name, const ImplicitSurfaceFunction& _f,
                          Color _color = Color::RED, const glm::mat4& _transform = glm::mat4(1.0)) :
        GeometryNode("Implicit Surface", _name, _transform), f_(_f), color_(_color)
    {
        std::strncpy(input_buffer_, _f.str.c_str(), sizeof(input_buffer_) - 1);
        input_buffer_[sizeof(input_buffer_) - 1] = '\0';
    }

    void initRenderer(Scene* scene) override;

    void renderUIBody(Scene* scene) override;
};

}
