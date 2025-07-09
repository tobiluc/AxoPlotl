#pragma once

#include "AxoPlotl/geometry/nodes/GeometryNode.h"

namespace AxoPlotl
{

struct GradientField
{
    std::function<float(Vec3f)> func;
    std::function<Vec3f(Vec3f)> grad;
    AABB bounds = {-5,5,-5,5,-5,5};

    GradientField(const std::function<float(Vec3f)>& f, double eps=1e-6) {
        setFunction(f, eps);
    }

    inline void setFunction(const std::function<float(Vec3f)>& f, double eps=1e-6) {
        func = f;

        // Approx
        Vec3f dx(eps,0,0);
        Vec3f dy(0,eps,0);
        Vec3f dz(0,0,eps);
        grad = [dx,dy,dz,this](const Vec3f& v) {
            return Vec3f(
                func(v+dx)-func(v-dx),
                func(v+dy)-func(v-dy),
                func(v+dz)-func(v-dz)
            );
        };
    }

    inline Vec3f operator()(const Vec3f& v) const {
        return grad(v);
    }
};

class VectorFieldNode : public GeometryNode
{
private:
    GradientField field_;
    int resolution_ = 16;
    float scale_factor_ = 0.4f;

public:
    VectorFieldNode(const GradientField& _field, const std::string& _name="New Gradient", const glm::mat4& _transform = glm::mat4(1.0)) :
        GeometryNode("Vector Field", _name, _transform), field_(_field)
    {
    }

    void addToRenderer(Scene* scene) override;

    void renderUIBody(Scene* scene) override;
};

}
