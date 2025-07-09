#pragma once

#include "AxoPlotl/commons/Color.h"
#include "AxoPlotl/geometry/Surface.h"
#include "AxoPlotl/geometry/nodes/GeometryNode.h"

namespace AxoPlotl
{

class ExplicitCurveNode : public GeometryNode
{
private:
    ExplicitCurveFunction f_;
    ColorFunction2f color_;
    int resolution_ = 256;
    char input_buffer_x_[512];
    char input_buffer_y_[512];
    char input_buffer_z_[512];

public:
    ExplicitCurveNode(const std::string& _name, const ExplicitCurveFunction& _f,
                        Color _color = Color::BLUE, const glm::mat4& _transform = glm::mat4(1.0)) :
        GeometryNode("Explicit Curve", _name, _transform),  f_(_f), color_(_color)
    {
        std::strncpy(input_buffer_x_, f_.str_x.c_str(), sizeof(input_buffer_x_));
        std::strncpy(input_buffer_y_, f_.str_y.c_str(), sizeof(input_buffer_y_));
        std::strncpy(input_buffer_z_, f_.str_z.c_str(), sizeof(input_buffer_z_));
        input_buffer_x_[sizeof(input_buffer_x_)-1] = '\0';
        input_buffer_y_[sizeof(input_buffer_y_)-1] = '\0';
        input_buffer_z_[sizeof(input_buffer_z_)-1] = '\0';
    }

    void addToRenderer(Scene* scene) override;

    void renderUIBody(Scene* scene) override;
};

}
