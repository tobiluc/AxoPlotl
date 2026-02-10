#pragma once

#include "AxoPlotl/geometry/Surface.h"
#include "AxoPlotl/geometry/nodes/GeometryNode.h"

namespace AxoPlotl
{

class ImplicitSurfaceNode : public GeometryNode
{
private:
    ImplicitSurfaceFunction f_;
    Vec3f color_;
    int initial_resolution_ = 16;
    int octree_depth_ = 2;
    char input_buffer_[1024];

public:
    ImplicitSurfaceNode(const std::string& _name, const ImplicitSurfaceFunction& _f,
                        Vec3f _color = Vec3f(1,0,0), const glm::mat4& _transform = glm::mat4(1.0)) :
        GeometryNode("Implicit Surface", _name, _transform), f_(_f), color_(_color)
    {
        std::strncpy(input_buffer_, _f.str.c_str(), sizeof(input_buffer_) - 1);
        input_buffer_[sizeof(input_buffer_) - 1] = '\0';
    }

    void init(Scene* scene) override;

    void renderUIBody(Scene* scene) override;

    inline std::pair<Vec3f,Vec3f> getBBox() override {
        return {Vec3f(f_.xMin,f_.yMin,f_.zMin),Vec3f(f_.xMax,f_.yMax,f_.zMax)};
    }
};

}
