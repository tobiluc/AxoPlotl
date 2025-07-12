#include "ExplicitCurveNode.h"
#include "AxoPlotl/algorithms/parsing/reverse_polish.h"

namespace AxoPlotl
{

void ExplicitCurveNode::renderUIBody(Scene* scene)
{
    //-------------------
    // Text
    //-------------------
    ImGui::NewLine();
    ImGui::Text("x(t) = ");
    ImGui::SameLine();
    if (ImGui::InputText("##x", input_buffer_x_, sizeof(input_buffer_x_))) {} // x changed
    ImGui::Text("y(t) = ");
    ImGui::SameLine();
    if (ImGui::InputText("##y", input_buffer_y_, sizeof(input_buffer_y_))) {}// y changed
    ImGui::Text("z(t) = ");
    ImGui::SameLine();
    if (ImGui::InputText("##z", input_buffer_z_, sizeof(input_buffer_z_))) {}// z changed

    //-------------------
    // Range Options
    //-------------------
    ImGui::InputFloat2("t Range", &f_.tMin);
    ImGui::SliderInt("Resolution", &resolution_, 16, 1024);

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

        std::function<Vec3f(float)> func = [&](float t) {
            vars["t"] = t;
            return Vec3f(
                Parsing::evaluate(rpn_x, vars, funcs),
                Parsing::evaluate(rpn_y, vars, funcs),
                Parsing::evaluate(rpn_z, vars, funcs)
                );
        };

        // Update renderer
        this->f_.f = func;
        this->initRenderer(scene);
    }
}

void ExplicitCurveNode::initRenderer(Scene* scene)
{
    GL::MeshRenderer::Data data;

    std::vector<std::pair<float,Vec3f>> pts;
    samplePoints(f_, pts, resolution_);

    for (uint i = 0; i < pts.size(); ++i) {
        data.lineAttribs.push_back(GL::MeshRenderer::VertexLineAttrib{.position = pts[i].second, .color = Color::WHITE});
    }
    for (uint i = 0; i < pts.size()-1; ++i) {
        data.lineIndices.push_back(i);
        data.lineIndices.push_back(i+1);
    }
    mesh_renderer_.updateData(data);
}

}
