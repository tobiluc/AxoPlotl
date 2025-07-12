#pragma once

#include "tokens.h"

namespace AxoPlotl::Parsing
{

struct RPN
{
    std::vector<Token> tokens;
};

void reversePolish(const std::vector<Token>& tokens, RPN& rpn);

using Variables = std::unordered_map<std::string, double>;
using FunctionArgs = std::vector<double>;
using FunctionImpl = std::function<double(const FunctionArgs&)>;
using Functions = std::unordered_map<std::string, FunctionImpl>;

double evaluate(const RPN& rpn, const Variables& vars, const Functions& funcs);

void registerCommons(Variables& vars, Functions& funcs);

}
