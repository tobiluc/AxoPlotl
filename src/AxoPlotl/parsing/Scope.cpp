#include "Scope.h"
#include "AxoPlotl/parsing/tree.h"
#include <iostream>
#include <ostream>

namespace AxoPlotl
{

Scope::Scope(std::unique_ptr<Scope> parent) :
    parent(std::move(parent))
{
    registerConstants();
}

bool Scope::hasVariable(const std::string& name)
{
    return variables.contains(name);
}

// bool Scope::hasFunction(const std::string& name)
// {
//     return functions.contains(name);
// }

const Scope::Variable Scope::getVariable(const std::string& name)
{
    if (hasVariable(name)) return variables.at(name);
    return std::make_shared<Parsing::EmptyNode>();
}

// const Scope::Function Scope::getFunction(const std::string& name)
// {
//     if (hasFunction(name)) return functions.at(name);
//     return {{}, std::make_shared<Parsing::EmptyNode>()};
// }

void Scope::setVariable(const std::string& name, Variable expr)
{
    if (hasVariable(name))
    {
        std::cerr << "Variable already exists" << std::endl;
        return;
    }
    variables[name] = expr;
}

// void Scope::setFunction(const std::string& name, Function expr)
// {
//     if (hasFunction(name))
//     {
//         std::cerr << "Function already exists" << std::endl;
//         return;
//     }
//     functions[name] = expr;
// }

void Scope::registerConstants()
{
    setVariable("pi", std::make_shared<Parsing::ScalarNode>(Parsing::ScalarNode(M_PI)));
    setVariable("e", std::make_shared<Parsing::ScalarNode>(Parsing::ScalarNode(M_E)));

    // TODO - Register Functions here instead of defining them in tree.cpp
}

}
