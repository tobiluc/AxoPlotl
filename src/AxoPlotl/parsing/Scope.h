#ifndef SCOPE_H
#define SCOPE_H

#include <unordered_map>
#include "tree.h"

namespace AxoPlotl
{

class Scope
{
using Variable = std::shared_ptr<Parsing::ASTNode>;
using Function = std::pair<std::vector<std::string>, std::shared_ptr<Parsing::ASTNode>>;

private:
    std::unique_ptr<Scope> parent;

    std::unordered_map<std::string, Variable> variables;
    std::unordered_map<std::string, Function> functions;

public:
    Scope(std::unique_ptr<Scope> parent = nullptr);

    bool hasVariable(const std::string& name);

    bool hasFunction(const std::string& name);

    const Variable getVariable(const std::string& name);

    const Function getFunction(const std::string& name);

    void setVariable(const std::string& name, Variable expr);

    void setFunction(const std::string& name, Function expr);
};

}

#endif // SCOPE_H
