#pragma once

#include "AxoPlotl/geometry/Surface.h"
#include "AxoPlotl/geometry/nodes/GeometryNode.h"

namespace AxoPlotl
{

class ExplicitSurfaceNode : public GeometryNode
{
private:
    std::pair<Vec3f,Vec3f> bbox_;
    ExplicitSurfaceFunction f_;
    ColorFunction2f color_;
    int resolution_ = 16;
    bool continous_evaluation_ = false;
    char input_buffer_x_[512];
    char input_buffer_y_[512];
    char input_buffer_z_[512];

public:
    ExplicitSurfaceNode(const std::string& _name, const ExplicitSurfaceFunction& _f,
                          Color _color = Color::BLUE, const glm::mat4& _transform = glm::mat4(1.0)) :
        GeometryNode("Explicit Surface", _name, _transform),  f_(_f), color_(_color)
    {
        std::strncpy(input_buffer_x_, f_.str_x.c_str(), sizeof(input_buffer_x_));
        std::strncpy(input_buffer_y_, f_.str_y.c_str(), sizeof(input_buffer_y_));
        std::strncpy(input_buffer_z_, f_.str_z.c_str(), sizeof(input_buffer_z_));
        input_buffer_x_[sizeof(input_buffer_x_)-1] = '\0';
        input_buffer_y_[sizeof(input_buffer_y_)-1] = '\0';
        input_buffer_z_[sizeof(input_buffer_z_)-1] = '\0';

        mesh_renderer_.settings().useGlobalTriangleColor = false;
        mesh_renderer_.settings().useDataForLineColor = true;
    }

    void initRenderer(Scene* scene) override;

    void renderUIBody(Scene* scene) override;

    inline std::pair<Vec3f,Vec3f> getBBox() override {
        return bbox_;
    }
};

}
