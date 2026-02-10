#pragma once

#include "AxoPlotl/geometry/Surface.h"
#include "GeometryNode.h"

namespace AxoPlotl
{

class SphericalHarmonicNode : public GeometryNode
{
private:
    SphericalHarmonicFunction f_;
    char input_buffer_[1024];
    int resolution_ = 32;

public:
    SphericalHarmonicNode(const SphericalHarmonicFunction& _f, const std::string& _name="SH", const glm::mat4& _transform = glm::mat4(1.0)) :
        GeometryNode("Spherical Harmonic", _name, _transform), f_(_f)
    {
        std::strncpy(input_buffer_, _f.str.c_str(), sizeof(input_buffer_) - 1);
        input_buffer_[sizeof(input_buffer_) - 1] = '\0';
    }

    void init(Scene* scene) override;

    void renderUIBody(Scene* scene) override;

    inline std::pair<Vec3f,Vec3f> getBBox() override {
        return {Vec3f(-1,-1,-1),Vec3f(1,1,1)};
    }
};

}
