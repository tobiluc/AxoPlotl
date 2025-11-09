#pragma once

#include "AxoPlotl/algorithms/parsing/reverse_polish.h"
#include "AxoPlotl/algorithms/parsing/tokens.h"
#include "glm/ext/vector_float3.hpp"
#include "imgui.h"

namespace AxoPlotl::Interface
{

void request_input_xyz(
    char* input_buffer_x,
    char* input_buffer_y,
    char* input_buffer_z
)
{
    //-------------------
    // Text
    //-------------------
    ImGui::NewLine();
    ImGui::Text("x(t) = ");
    ImGui::SameLine();
    if (ImGui::InputText("##x", input_buffer_x, sizeof(input_buffer_x))) {} // x changed
    ImGui::Text("y(t) = ");
    ImGui::SameLine();
    if (ImGui::InputText("##y", input_buffer_y, sizeof(input_buffer_y))) {}// y changed
    ImGui::Text("z(t) = ");
    ImGui::SameLine();
    if (ImGui::InputText("##z", input_buffer_z, sizeof(input_buffer_z))) {}// z changed

    //-------------------
    // Range Options
    //-------------------
    //ImGui::InputFloat2("t Range", &f_.tMin);
    //ImGui::SliderInt("Resolution", &resolution_, 16, 1024);

    //-------------------
    // Update
    //-------------------
    if (ImGui::Button("Confirm")) {
        // Parse Input Text
        auto tokens_x = Parsing::tokenize(input_buffer_x);
        auto tokens_y = Parsing::tokenize(input_buffer_y);
        auto tokens_z = Parsing::tokenize(input_buffer_z);

        Parsing::RPN rpn_x;
        Parsing::reversePolish(tokens_x, rpn_x);
        Parsing::RPN rpn_y;
        Parsing::reversePolish(tokens_y, rpn_y);
        Parsing::RPN rpn_z;
        Parsing::reversePolish(tokens_z, rpn_z);
        Parsing::Variables vars;
        Parsing::Functions funcs;
        Parsing::registerCommons(vars, funcs);

        std::function<glm::vec3(float)> func = [&](float t) {
            vars["t"] = t;
            return glm::vec3(
                Parsing::evaluate(rpn_x, vars, funcs),
                Parsing::evaluate(rpn_y, vars, funcs),
                Parsing::evaluate(rpn_z, vars, funcs)
            );
        };
    }
}

}
