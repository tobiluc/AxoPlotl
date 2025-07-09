#include "VectorFieldNode.h"

namespace AxoPlotl
{

void VectorFieldNode::addToRenderer(Scene* scene)
{
    //bbox_.compute(mesh_.vertices());

    std::vector<Rendering::Line> lines;

    for (int ix = 0; ix <= resolution_; ++ix) {
        for (int iy = 0; iy <= resolution_; ++iy) {
            for (int iz = 0; iz <= resolution_; ++iz) {
                Vec3f p(
                    field_.bounds.x0 + ix/(float)resolution_*(field_.bounds.x1-field_.bounds.x0),
                    field_.bounds.y0 + iy/(float)resolution_*(field_.bounds.y1-field_.bounds.y0),
                    field_.bounds.z0 + iz/(float)resolution_*(field_.bounds.z1-field_.bounds.z0)
                );

                Vec3f d = glm::normalize(field_(p));
                lines.emplace_back(p, p+scale_factor_*d, getColorOnSphere(d.x, d.y, d.z));
            }
        }
    }

    renderer_.addLines(lines, renderLoc_);
}

void VectorFieldNode::renderUIBody(Scene* scene)
{
    ImGui::InputFloat("Scale Factor", &scale_factor_);

    // if (ImGui::Button("Confirm")) {
    //     renderer_.clear(renderLoc_);
    //     this->addToRenderer(scene);
    // }
}

}
