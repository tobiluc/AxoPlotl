#include "VectorFieldNode.h"
#include "AxoPlotl/algorithms/parsing/reverse_polish.h"

namespace AxoPlotl
{

void VectorFieldNode::initRenderer(Scene* scene)
{
    //bbox_.compute(mesh_.vertices());

    GL::MeshRenderer::Data data;

    for (int ix = 0; ix <= resolution_; ++ix) {
        for (int iy = 0; iy <= resolution_; ++iy) {
            for (int iz = 0; iz <= resolution_; ++iz) {
                Vec3f p(
                    field_.xMin + ix/(float)resolution_*(field_.xMax-field_.xMin),
                    field_.yMin + iy/(float)resolution_*(field_.yMax-field_.zMin),
                    field_.zMin + iz/(float)resolution_*(field_.zMax-field_.zMin)
                );

                Vec3f d = glm::normalize(field_(p));
                Color color = getColorOnSphere(d.x, d.y, d.z);
                int v = data.lineAttribs.size();
                data.lineAttribs.push_back({.position = p-0.5f*scale_factor_*d, .color = color});
                data.lineAttribs.push_back({.position = p+0.5f*scale_factor_*d, .color = color});
                data.lineIndices.push_back(v+0);
                data.lineIndices.push_back(v+1);
            }
        }
    }

    mesh_renderer_.updateData(data);

    // renderer_.addLines(lines, renderLoc_);
}

void VectorFieldNode::renderUIBody(Scene* scene)
{
    //-------------------
    // Text
    //-------------------
    ImGui::NewLine();
    ImGui::Text("x(x,y,z) = ");
    ImGui::SameLine();
    if (ImGui::InputText("##x", input_buffer_x_, sizeof(input_buffer_x_))) {} // x changed
    ImGui::Text("y(x,y,z) = ");
    ImGui::SameLine();
    if (ImGui::InputText("##y", input_buffer_y_, sizeof(input_buffer_y_))) {}// y changed
    ImGui::Text("z(x,y,z) = ");
    ImGui::SameLine();
    if (ImGui::InputText("##z", input_buffer_z_, sizeof(input_buffer_z_))) {}// z changed

    // Options
    ImGui::InputFloat("Scale Factor", &scale_factor_);

    ImGui::InputFloat2("x Range", &field_.xMin);
    ImGui::InputFloat2("y Range", &field_.yMin);
    ImGui::InputFloat2("z Range", &field_.zMin);

    ImGui::SliderInt("Resolution", &resolution_, 1.0f, 128.0f);

    //-------------------
    // Update
    //-------------------
    if (ImGui::Button("Confirm")) {
        // Parse Input Text
        auto tokens_x = Parsing::tokenize(input_buffer_x_);
        auto tokens_y = Parsing::tokenize(input_buffer_y_);
        auto tokens_z = Parsing::tokenize(input_buffer_z_);

        Parsing::RPN rpn_x;
        Parsing::reversePolish(tokens_x, rpn_x);
        Parsing::RPN rpn_y;
        Parsing::reversePolish(tokens_y, rpn_y);
        Parsing::RPN rpn_z;
        Parsing::reversePolish(tokens_z, rpn_z);
        Parsing::Variables vars;
        Parsing::Functions funcs;
        Parsing::registerCommons(vars, funcs);

        std::function<Vec3f(Vec3f)> func = [&](const Vec3f& p) {
            vars["x"] = p.x;
            vars["y"] = p.y;
            vars["z"] = p.z;
            return Vec3f(
                Parsing::evaluate(rpn_x, vars, funcs),
                Parsing::evaluate(rpn_y, vars, funcs),
                Parsing::evaluate(rpn_z, vars, funcs)
                );
        };

        // Update renderer
        this->field_.grad = func;
        this->initRenderer(scene);
    }
}

}
