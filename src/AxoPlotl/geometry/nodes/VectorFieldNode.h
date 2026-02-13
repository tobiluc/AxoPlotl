#pragma once

#include "AxoPlotl/geometry/nodes/GeometryNode.h"

namespace AxoPlotl
{

struct GradientField
{
    std::function<Vec3f(Vec3f)> grad;
    float xMin = -5, xMax = 5, yMin = -5, yMax = 5, zMin = -5, zMax = 5;

    GradientField(const std::function<Vec3f(Vec3f)>& g) : grad(g) {
    }

    inline Vec3f operator()(const Vec3f& p) const {
        return grad(p);
    }
};

class VectorFieldNode : public GeometryNode
{
private:
    GradientField field_;
    int resolution_ = 16;
    float scale_factor_ = 0.4f;
    char input_buffer_x_[512] = "x";
    char input_buffer_y_[512] = "y";
    char input_buffer_z_[512] = "z";

public:
    VectorFieldNode(const GradientField& _field, const std::string& _name="New Gradient", const glm::mat4& _transform = glm::mat4(1.0)) :
        GeometryNode(_name, _transform), field_(_field)
    {
    }

    void init(Scene* scene) override;

    void renderUIBody(Scene* scene) override;
};

}
