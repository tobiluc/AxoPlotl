#include "ExplicitCurveNode.h"
#include "AxoPlotl/parsing/Scope.h"
#include "AxoPlotl/parsing/parsing.h"

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

        // Tranform ASTa into functiona of t
        auto root_x = Parsing::Parser(tokens_x).parse();
        auto root_y = Parsing::Parser(tokens_y).parse();
        auto root_z = Parsing::Parser(tokens_z).parse();

        std::function<Vec3f(float)> func = [&](float t) {
            Scope scope;
            scope.setVariable("t", std::make_shared<Parsing::ScalarNode>(t));

            auto value_x = root_x->evaluate(scope);
            auto value_y = root_y->evaluate(scope);
            auto value_z = root_z->evaluate(scope);

            return Vec3f(
                std::visit([](auto&& v) {
                    using T = std::decay_t<decltype(v)>;
                    if constexpr (std::is_same_v<T, double>) {return v;}
                    else {return 0.0;}
                }, value_x.val),
                std::visit([](auto&& v) {
                    using T = std::decay_t<decltype(v)>;
                    if constexpr (std::is_same_v<T, double>) {return v;}
                    else {return 0.0;}
                }, value_y.val),
                std::visit([](auto&& v) {
                    using T = std::decay_t<decltype(v)>;
                    if constexpr (std::is_same_v<T, double>) {return v;}
                    else {return 0.0;}
                }, value_z.val)
                );
        };

        // Update renderer
        this->f_.f = func;
        mesh_renderer_.deleteBuffers();
        this->initRenderer(scene);
    }
}

void ExplicitCurveNode::initRenderer(Scene* scene)
{
    PolyhedralMesh mesh;

    LineMesh lines;
    createLines(f_, lines, resolution_);

    for (const auto& p : lines.vertices) {
        mesh.add_vertex(toVec3<OVM::Vec3d>(p));
    }

    for (const auto& l : lines.lines) {
        mesh.add_edge(OVM::VH(l[0]), OVM::VH(l[1]));
    }

    mesh_renderer_.init(mesh);

    // std::vector<Rendering::Line> lines;
    // for (uint i = 0; i < mesh.lines.size(); ++i) {
    //     lines.emplace_back(
    //         mesh.vertices[mesh.lines[i][0]],
    //         mesh.vertices[mesh.lines[i][1]],
    //         ui_color_ // TODO: Display correct color
    //         );
    // }
    // renderer_.addLines(lines, renderLoc_);
}

}
