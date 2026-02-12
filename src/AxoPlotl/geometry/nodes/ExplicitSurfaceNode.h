#pragma once

#include "AxoPlotl/geometry/Surface.h"
#include "AxoPlotl/geometry/nodes/GeometryNode.h"

namespace AxoPlotl
{

class ExplicitSurfaceNode : public GeometryNode
{
private:
    ExplicitSurfaceFunction f_;
    std::function<Vec3f(float,float)> color_;
    int resolution_ = 16;
    bool continous_evaluation_ = false;
    char input_buffer_x_[512];
    char input_buffer_y_[512];
    char input_buffer_z_[512];

public:
    ExplicitSurfaceNode(const std::string& _name, const ExplicitSurfaceFunction& _f,
                        Vec3f _color = Vec3f(0,0,1), const glm::mat4& _transform = glm::mat4(1.0)) :
        GeometryNode(_name, _transform),  f_(_f), color_([&](float,float){return _color;})
    {
        std::strncpy(input_buffer_x_, f_.str_x.c_str(), sizeof(input_buffer_x_));
        std::strncpy(input_buffer_y_, f_.str_y.c_str(), sizeof(input_buffer_y_));
        std::strncpy(input_buffer_z_, f_.str_z.c_str(), sizeof(input_buffer_z_));
        input_buffer_x_[sizeof(input_buffer_x_)-1] = '\0';
        input_buffer_y_[sizeof(input_buffer_y_)-1] = '\0';
        input_buffer_z_[sizeof(input_buffer_z_)-1] = '\0';
    }

    void init(Scene* scene) override;

    void renderUIBody(Scene* scene) override;
};

}
