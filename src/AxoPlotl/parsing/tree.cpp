#include "tree.h"
#include "Scope.h"

namespace AxoPlotl::Parsing
{

inline void printDepthPrefix(uint depth)
{
    for (uint i = 0; i < depth; ++i) std::cout << " ";
    std::cout << "L ";
}

//================
// Scalar Node
//================

ScalarNode::ScalarNode(double val) : value(val) {}

const EvalValue ScalarNode::evaluate(Scope& context) const
{
    return value;
}

EvalFunction ScalarNode::compile() const
{
    double value = this->value;
    return [value](Scope& context) {return value;};
}

void ScalarNode::print(uint depth) const
{
    printDepthPrefix(depth);
    std::cout << " Scalar: " << value;
}

//================
// Point Node
//================

PointNode::PointNode(double x, double y, double z) :
    p(x, y, z)
{}

const EvalValue PointNode::evaluate(Scope& context) const
{
    return p;
}

EvalFunction PointNode::compile() const
{
    Vec3d value = this->p;
    return [value](Scope& context) {return value;};
}

void PointNode::print(uint depth) const
{
    printDepthPrefix(depth);
    std::cout << " Point: " << "(" << p.x << ", " << p.y << ", " << p.z << ")";
}

//================
// List Node
//================

ListNode::ListNode(const std::vector<std::shared_ptr<ASTNode>>& children) :
    children(children)
{}

const EvalValue ListNode::evaluate(Scope& context) const
{
    EvalValueList values;
    //std::vector<std::shared_ptr<EvalValue>> values;
    values.reserve(children.size());
    for (auto& child : children) {values.push_back(child->evaluate(context));}
    return values;
}

EvalFunction ListNode::compile() const
{
    std::vector<EvalFunction> childrenFuncs;
    for (const auto& child : children) {
        childrenFuncs.push_back(child->compile());
    }

    return [childrenFuncs](Scope& context) {
        std::vector<EvalValue> vals;
        for (const auto& f : childrenFuncs) {vals.push_back(f(context));}
        return vals;
    };
}

void ListNode::print(uint depth) const
{
    printDepthPrefix(depth);
    std::cout << " List" << std::endl;
    for (uint i = 0; i < children.size(); ++i)
    {
        children[i]->print(depth + 1);
        if (i < children.size()-1) std::cout << std::endl;
    }
}

//================
// Variable Node
//================

VariableNode::VariableNode(const std::string& name) :
    name(name)
{}

const EvalValue VariableNode::evaluate(Scope& context) const
{
    return context.getVariable(name)->evaluate(context);
}

EvalFunction VariableNode::compile() const
{
    std::string name = this->name;
    return [name](Scope& context) {
        return context.getVariable(name)->evaluate(context);
    };
}

void VariableNode::print(uint depth) const
{
    printDepthPrefix(depth);
    std::cout << " Variable \"" << name << "\"";
}

//=================
// Binary Op Node
//=================

BinaryOpNode::BinaryOpNode(Token::Type op, std::shared_ptr<ASTNode> left, std::shared_ptr<ASTNode> right) :
    op(op),
    left(left),
    right(right)
{}

const EvalValue BinaryOpNode::evaluate(Scope& context) const
{
    auto leftValue = left->evaluate(context);
    auto rightValue = right->evaluate(context);

    return std::visit([this](auto&& a, auto&& b) -> EvalValue {
        using A = std::decay_t<decltype(a)>;
        using B = std::decay_t<decltype(b)>;

        // Scalar-Scalar
        if constexpr (std::is_same_v<A, double> && std::is_same_v<B, double>) {
            switch (op) {
            case Token::Type::PLUS:  return EvalValue(a + b);
            case Token::Type::MINUS: return EvalValue(a - b);
            case Token::Type::TIMES: return EvalValue(a * b);
            case Token::Type::POW:   return EvalValue(std::pow(a, b));
            case Token::Type::DIV: return EvalValue(a / b);
            default:
                std::cerr << "Unknown binary operator for scalars" << std::endl;
                return 0.0;
            }
        }

        // Vector-Vector
        else if constexpr (std::is_same_v<A, Vec3d> && std::is_same_v<B, Vec3d>) {
            switch (op) {
            case Token::Type::PLUS:  return EvalValue(a + b);
            case Token::Type::MINUS: return EvalValue(a - b);
            case Token::Type::TIMES: return EvalValue(a * b);
            case Token::Type::DIV: return EvalValue(a / b);
            default:
                std::cerr << "Unknown binary operator for vectors" << std::endl;
                return 0.0;
            }
        }

        // Handle unsupported combinations
        else {
            std::cerr << "Incompatible types for binary operator" << std::endl;
            return 0.0;
        }

    }, leftValue.val, rightValue.val);
}

EvalFunction BinaryOpNode::compile() const
{
    auto leftFunc = left->compile();
    auto rightFunc = right->compile();

    // TODO: Work with Non-Scalar Types

    switch (op)
    {
    case Token::Type::PLUS:
        return [leftFunc, rightFunc](Scope& ctx) {
            return leftFunc(ctx).value<double>() + rightFunc(ctx).value<double>();
        };
    case Token::Type::MINUS:
        return [leftFunc, rightFunc](Scope& ctx) {
            return leftFunc(ctx).value<double>() - rightFunc(ctx).value<double>();
        };
    case Token::Type::TIMES:
        return [leftFunc, rightFunc](Scope& ctx) {
            return leftFunc(ctx).value<double>() * rightFunc(ctx).value<double>();
        };
    case Token::Type::DIV:
        return [leftFunc, rightFunc](Scope& ctx) {
            double rval = rightFunc(ctx).value<double>();
            if (rval == 0.0) throw std::runtime_error("Division by zero");
            return leftFunc(ctx).value<double>() / rval;
        };
    case Token::Type::POW:
        return [leftFunc, rightFunc](Scope& ctx) {
            return std::pow(leftFunc(ctx).value<double>(), rightFunc(ctx).value<double>());
        };
    default:
        throw std::runtime_error("Unknown binary operator");
    }
}

void BinaryOpNode::print(uint depth) const
{
    printDepthPrefix(depth);
    std::cout << " Binary Operation: " << (int)(op) << std::endl;
    left->print(depth+1);
    std::cout << std::endl;
    right->print(depth+1);
}

//==================
// Unary Op Node
//==================

UnaryOpNode::UnaryOpNode(Token::Type op, std::shared_ptr<ASTNode> child) :
    op(op),
    child(child)
{}

const EvalValue UnaryOpNode::evaluate(Scope& context) const
{
    auto value = child->evaluate(context);

    return std::visit([this](auto&& v) -> EvalValue {
        using T = std::decay_t<decltype(v)>;

        // Scalar Operations
        if constexpr (std::is_same_v<T, double>) {
            switch (op) {
            case Token::Type::PLUS:  return EvalValue(v);
            case Token::Type::MINUS: return EvalValue(-v);
            default:
                std::cerr << "Unknown unary operator for scalar" << std::endl;
                return 0.0;
            }
        }
        // Vector Operations
        else if constexpr (std::is_same_v<T, Vec3d>) {
            switch (op) {
            case Token::Type::PLUS:  return EvalValue(v); // No change
            case Token::Type::MINUS: return EvalValue(Vec3d(-v.x, -v.y, -v.z));
            default:
                std::cerr << "Unknown unary operator for vector" << std::endl;
                return 0.0;
            }
        }
        else {
            std::cerr << "Unary operator not supported for this type" << std::endl;
            return 0.0;
        }

    }, value.val);
}

EvalFunction UnaryOpNode::compile() const
{
    auto func = child->compile();

    // TODO: Non-Scalar Types

    switch (op)
    {
    case Token::Type::PLUS:
        return [func](Scope& ctx) {
            auto val = func(ctx);
            return val;
        };
    case Token::Type::MINUS:
        return [func](Scope& ctx) {
            auto val = func(ctx);
            return -val.value<double>();
        };
    default:
        return [](Scope& ctx) {return 0.0f;};
    }
}

void UnaryOpNode::print(uint depth) const
{
    printDepthPrefix(depth);
    std::cout << " Unary Operation: " << (int)(op) << std::endl;
    child->print(depth+1);
}

//==================
// Assign Node
//==================

// AssignNode::AssignNode(const std::string& name, std::shared_ptr<ASTNode> right) :
//     name(name),
//     right(right)
// {}

// const EvalValue AssignNode::evaluate(Scope& context) const
// {
//     context.setVariable(name, right);
//     return right->evaluate(context);
// }

// void AssignNode::print(uint depth) const
// {
//     printDepthPrefix(depth);
//     std::cout << " Assign \"" << name << "\" = " << std::endl;
//     right->print(depth+1);
// }

//===========================
// Function Definition Node
//===========================

// FunctionAssignNode::FunctionAssignNode(const std::string& name, const std::vector<std::string>& params, std::shared_ptr<ASTNode> func) :
//     name(name), params(params), func(func)
// {}

// const EvalValue FunctionAssignNode::evaluate(Scope& context) const
// {
//     context.setFunction(name, {params, func});
//     return func->evaluate(context);
// }

// void FunctionAssignNode::print(uint depth) const
// {
//     printDepthPrefix(depth);
//     std::cout << " FuncDef \"" << name << "(";
//     for (uint i = 0; i < params.size(); ++i) {std::cout << params[i]; if (i < params.size()-1) std::cout << ", ";}
//     std::cout << ")\" = " << std::endl;
//     func->print(depth + 1);
// }

//===========================
// Function Call Node
//===========================

FunctionCallNode::FunctionCallNode(const std::string& name, const std::vector<std::shared_ptr<ASTNode>>& args) :
    name(name), args(args)
{}

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

const EvalValue FunctionCallNode::evaluate(Scope& context) const
{
    std::vector<EvalValue> argsValues;
    for (const auto& arg : args) {
        argsValues.push_back(arg->evaluate(context));
    }
    uint nargs = argsValues.size();

    std::unordered_map<std::string, std::function<double(double)>> functions1d1d;
    functions1d1d["sin"] = [](double x) {return std::sin(x);};
    functions1d1d["cos"] = [](double x) {return std::cos(x);};
    functions1d1d["log"] = [](double x) {return std::log(x);};
    functions1d1d["ln"] = functions1d1d["log"];
    functions1d1d["exp"] = [](double x) {return std::exp(x);};
    functions1d1d["sqrt"] = [](double x) {return std::sqrt(x);};

    // TODO: Move the logic of different functions to Scope.cpp
    // to define custom functions at runtime
    // TODO: Supportfunctions with arbitrary return type and args/arg count

    if (!functions1d1d.contains(name)) {
        // Unknown function
        return 0.0;
    }

    return functions1d1d.at(name)(argsValues.at(0).value<double>());
}

EvalFunction FunctionCallNode::compile() const
{
    std::vector<EvalFunction> argFuncs;
    for (const auto& arg : args) {
        argFuncs.push_back(arg->compile());
    }

    // TODO: Make this work for n-ary functions with different return types

    std::unordered_map<std::string, std::function<double(double)>> functions1d1d;
    functions1d1d["sin"] = [](double x) {return std::sin(x);};
    functions1d1d["cos"] = [](double x) {return std::cos(x);};
    functions1d1d["log"] = [](double x) {return std::log(x);};
    functions1d1d["ln"] = functions1d1d["log"];
    functions1d1d["exp"] = [](double x) {return std::exp(x);};
    functions1d1d["sqrt"] = [](double x) {return std::sqrt(x);};

    if (!functions1d1d.contains(name)) {
        return [](Scope& context) {return 0.0f;};
    }

    auto func = functions1d1d[name];
    auto arg = argFuncs[0];
    return [arg,func](Scope& context) {
        auto tmp = arg(context);
        return func(tmp.value<double>());
    };
}

void FunctionCallNode::print(uint depth) const
{
    printDepthPrefix(depth);
    std::cout << " Function \"" << name << "\"" << std::endl;
    for (const auto& arg : args) {arg->print(depth+1);}
};

//==================
// Empty Node
//==================

EmptyNode::EmptyNode()
{}

const EvalValue EmptyNode::evaluate(Scope& context) const
{
    return 0.0;
}

EvalFunction EmptyNode::compile() const
{
    return [](Scope& context) {return 0.0f;};
}

void EmptyNode::print(uint depth) const
{}

}
