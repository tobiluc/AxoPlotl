#ifndef AXPLINPUT_H
#define AXPLINPUT_H

#include "AxoPlotl/parsing/tree.h"

namespace AxPl
{

class AxPlInput
{
public:
    AxPlInput(Scope& scope);

    void renderImGui();

private:
    std::unique_ptr<Parsing::ASTNode> root;
    char input_text_buffer[256];
    Scope& scope;
};

}

#endif // AXPLINPUT_H
