#include "AxPlInput.h"
#include <AxoPlotl/parsing/parsing.h>
#include <AxoPlotl/parsing/Scope.h>
#include "imgui.h"

namespace AxPl
{

AxPlInput::AxPlInput(Scope &scope) :
    scope(scope)
{

}

void AxPlInput::renderImGui()
{
    if (ImGui::InputText("Input", input_text_buffer, sizeof(input_text_buffer)))
    {
        // Text Changed
    }

    if (ImGui::Button("Confirm"))
    {
        auto tokens = Parsing::tokenize(input_text_buffer);
        for (auto& token : tokens) std::cout << token << std::endl;
        auto root = Parsing::Parser(tokens).parse();
        auto value = root->evaluate(scope);
        if (value)
        {
            root->print();
            std::cout << std::endl;
            value->print();
            std::cout << std::endl;
        } else {std::cout << "INVALID EVALUATION" << std::endl;}
    }
}

}
