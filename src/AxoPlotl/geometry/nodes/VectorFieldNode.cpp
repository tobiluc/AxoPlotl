#include "VectorFieldNode.h"
#include "AxoPlotl/utils/Utils.h"

namespace AxoPlotl
{

void VectorFieldNode::addToRenderer(Scene* scene)
{
    //bbox_.compute(mesh_.vertices());

    PolyhedralMesh mesh;

    for (int ix = 0; ix <= resolution_; ++ix) {
        for (int iy = 0; iy <= resolution_; ++iy) {
            for (int iz = 0; iz <= resolution_; ++iz) {
                Vec3f p(
                    field_.bounds.x0 + ix/(float)resolution_*(field_.bounds.x1-field_.bounds.x0),
                    field_.bounds.y0 + iy/(float)resolution_*(field_.bounds.y1-field_.bounds.y0),
                    field_.bounds.z0 + iz/(float)resolution_*(field_.bounds.z1-field_.bounds.z0)
                );

                Vec3f d = glm::normalize(field_(p));
                OVM::VH vh1 = mesh.add_vertex(toVec3<OVM::Vec3d>(p));
                OVM::VH vh2 = mesh.add_vertex(toVec3<OVM::Vec3d>(p+scale_factor_*d));
                mesh.add_edge(vh1, vh2);
                // getColorOnSphere(d.x, d.y, d.z);
            }
        }
    }

    mesh_renderer_.initFromMesh(mesh);

    // renderer_.addLines(lines, renderLoc_);
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
