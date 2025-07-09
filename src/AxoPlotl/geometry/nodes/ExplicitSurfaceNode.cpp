#include "ExplicitSurfaceNode.h"
#include "AxoPlotl/parsing/Scope.h"
#include "AxoPlotl/parsing/parsing.h"
#include "AxoPlotl/parsing/tokens.h"

namespace AxoPlotl
{

void ExplicitSurfaceNode::renderUIBody(Scene* scene)
{
    //-------------------
    // Text
    //-------------------
    ImGui::NewLine();
    ImGui::Text("x(u,v) = ");
    ImGui::SameLine();
    if (ImGui::InputText("##x", input_buffer_x_, sizeof(input_buffer_x_))) {} // x changed
    ImGui::Text("y(u,v) = ");
    ImGui::SameLine();
    if (ImGui::InputText("##y", input_buffer_y_, sizeof(input_buffer_y_))) {}// y changed
    ImGui::Text("z(u,v) = ");
    ImGui::SameLine();
    if (ImGui::InputText("##z", input_buffer_z_, sizeof(input_buffer_z_))) {}// z changed

    //-------------------
    // Range Options
    //-------------------
    ImGui::InputFloat2("u Range", &f_.uMin);
    ImGui::InputFloat2("v Range", &f_.vMin);
    ImGui::SliderInt("Resolution", &resolution_, 2, 64);

    //-------------------
    // Update
    //-------------------
    if (ImGui::Button("Confirm")) {
        // Parse Input Text
        auto tokens_x = Parsing::tokenize(input_buffer_x_);
        auto tokens_y = Parsing::tokenize(input_buffer_y_);
        auto tokens_z = Parsing::tokenize(input_buffer_z_);

        // Tranform ASTa into functiona of u, v
        auto root_x = Parsing::Parser(tokens_x).parse();
        auto root_y = Parsing::Parser(tokens_y).parse();
        auto root_z = Parsing::Parser(tokens_z).parse();

        std::function<Vec3f(float,float)> func = [&](float u, float v) {
            Scope scope;
            scope.setVariable("u", std::make_shared<Parsing::ScalarNode>(u));
            scope.setVariable("v", std::make_shared<Parsing::ScalarNode>(v));

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
        renderer_.clear(renderLoc_);
        this->addToRenderer(scene);
    }
}

void ExplicitSurfaceNode::addToRenderer(Scene* scene)
{
    TriangleMesh mesh;
    createTriangles(f_, mesh, resolution_);
    std::vector<Rendering::Triangle> tris;
    for (uint i = 0; i < mesh.triangles.size(); ++i) {
        tris.emplace_back(
            mesh.vertices[mesh.triangles[i][0]],
            mesh.vertices[mesh.triangles[i][1]],
            mesh.vertices[mesh.triangles[i][2]],
            ui_color_ // TODO: Display correct color
            );
    }
    renderer_.addTriangles(tris, renderLoc_);
}

}
