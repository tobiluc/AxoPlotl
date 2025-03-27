#include "tree.h"
#include "Scope.h"

namespace AxPl::Parsing
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

const std::shared_ptr<Value> ScalarNode::evaluate(Scope& context) const
{
    return std::make_shared<ScalarValue>(value);
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
    x(x), y(y), z(z)
{}

const std::shared_ptr<Value> PointNode::evaluate(Scope& context) const
{
    return std::make_shared<PointValue>(x, y, z);
}

void PointNode::print(uint depth) const
{
    printDepthPrefix(depth);
    std::cout << " Point: " << "(" << x << ", " << y << ", " << z << ")";
}

//================
// List Node
//================

ListNode::ListNode(const std::vector<std::shared_ptr<ASTNode>>& children) :
    children(children)
{}

const std::shared_ptr<Value> ListNode::evaluate(Scope& context) const
{
    std::vector<std::shared_ptr<Value>> values;
    values.reserve(children.size());
    for (auto& child : children) values.push_back(child->evaluate(context));
    return std::make_shared<ListValue>(values);
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

const std::shared_ptr<Value> VariableNode::evaluate(Scope& context) const
{
    return context.getVariable(name)->evaluate(context);
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

const std::shared_ptr<Value> BinaryOpNode::evaluate(Scope& context) const
{
    auto leftValue = left->evaluate(context);
    auto rightValue = right->evaluate(context);

    // Scalar Operations
    auto leftScalar = dynamic_cast<ScalarValue*>(leftValue.get());
    auto rightScalar = dynamic_cast<ScalarValue*>(rightValue.get());
    if (leftScalar && rightScalar)
    {
        double lval = leftScalar->getValue();
        double rval = rightScalar->getValue();
        switch (op)
        {
        case Token::Type::PLUS:
            return std::make_shared<ScalarValue>(lval + rval);
        case Token::Type::MINUS:
            return std::make_shared<ScalarValue>(lval - rval);
        case Token::Type::TIMES:
            return std::make_shared<ScalarValue>(lval * rval);
        case Token::Type::DIV:
            if (rval == 0) throw std::runtime_error("Division by zero!");
            return std::make_shared<ScalarValue>(lval / rval);
        case Token::Type::POW:
            return std::make_shared<ScalarValue>(std::pow(lval, rval));
        default:
            throw std::runtime_error("Unknown operator for scalars");
        }
    }

    // Vector Operations
    auto leftVec = dynamic_cast<PointValue*>(leftValue.get());
    auto rightVec = dynamic_cast<PointValue*>(rightValue.get());
    if (leftVec && rightVec)
    {
        auto lval = leftVec->getValue<std::array<double, 3>>();
        auto rval = rightVec->getValue<std::array<double, 3>>();
        switch (op)
        {
        case Token::Type::PLUS:
            return std::make_shared<PointValue>(
                lval[0] + rval[0],
                lval[1] + rval[1],
                lval[2] + rval[2]
                );
        case Token::Type::MINUS:
            return std::make_shared<PointValue>(
                lval[0] - rval[0],
                lval[1] - rval[1],
                lval[2] - rval[2]
                );
        case Token::Type::TIMES:
            return std::make_shared<PointValue>(
                lval[0] * rval[0],
                lval[1] * rval[1],
                lval[2] * rval[2]
                );
        case Token::Type::DIV:
            if (!rval[0] || !rval[1] || !rval[2]) throw std::runtime_error("Division by zero!");
            return std::make_shared<PointValue>(
                lval[0] / rval[0],
                lval[1] / rval[1],
                lval[2] / rval[2]
                );
        // case Token::Type::PERCENT:
        //     return std::make_shared<PointValue>(
        //         lval[1] * rval[2] - lval[2] * rval[1],
        //         lval[2] * rval[0] - lval[0] * rval[2],
        //         lval[0] * rval[1] - lval[1] * rval[0]
        //     );
        default:
            throw std::runtime_error("Unknown operator for vectors");
        }
    }

    std::cerr << "Incompatible types for binary operator:\n";
    leftValue->print();
    std::cout << std::endl;
    rightValue->print();
    return std::make_shared<NullValue>();
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

const std::shared_ptr<Value> UnaryOpNode::evaluate(Scope& context) const
{
    auto value = child->evaluate(context);

    // Scalar Operations
    auto scalar = dynamic_cast<ScalarValue*>(value.get());
    if (scalar)
    {
        double val = scalar->getValue();
        switch (op)
        {
        case Token::Type::PLUS:
            return std::make_shared<ScalarValue>(val);
        case Token::Type::MINUS:
            return std::make_shared<ScalarValue>(-val);
        default:
            throw std::runtime_error("Unknown unary operator for scalars");
        }
    }

    // Vector Operations
    auto vec = dynamic_cast<PointValue*>(value.get());
    if (vec)
    {
        auto val = vec->getValue<std::array<double, 3>>();
        switch (op)
        {
        case Token::Type::PLUS:
            return std::make_shared<PointValue>(
                val[0],
                val[1],
                val[2]
                );
        case Token::Type::MINUS:
            return std::make_shared<PointValue>(
                -val[0],
                -val[1],
                -val[2]
                );
        default:
            throw std::runtime_error("Unknown unary operator for vectors");
        }
    }

    throw std::runtime_error("Incompatible types for operator");
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

AssignNode::AssignNode(const std::string& name, std::shared_ptr<ASTNode> right) :
    name(name),
    right(right)
{}

const std::shared_ptr<Value> AssignNode::evaluate(Scope& context) const
{
    context.setVariable(name, right);
    return right->evaluate(context);
}

void AssignNode::print(uint depth) const
{
    printDepthPrefix(depth);
    std::cout << " Assign \"" << name << "\" = " << std::endl;
    right->print(depth+1);
}

//===========================
// Function Definition Node
//===========================

FunctionAssignNode::FunctionAssignNode(const std::string& name, const std::vector<std::string>& params, std::shared_ptr<ASTNode> func) :
    name(name), params(params), func(func)
{}

const std::shared_ptr<Value> FunctionAssignNode::evaluate(Scope& context) const
{
    context.setFunction(name, {params, func});
    return func->evaluate(context);
}

void FunctionAssignNode::print(uint depth) const
{
    printDepthPrefix(depth);
    std::cout << " FuncDef \"" << name << "(";
    for (uint i = 0; i < params.size(); ++i) {std::cout << params[i]; if (i < params.size()-1) std::cout << ", ";}
    std::cout << ")\" = " << std::endl;
    func->print(depth + 1);
}

//===========================
// Function Call Node
//===========================

FunctionCallNode::FunctionCallNode(const std::string& name, const std::vector<std::shared_ptr<ASTNode>>& args) :
    name(name), args(args)
{}

const std::shared_ptr<Value> FunctionCallNode::evaluate(Scope& context) const
{
    if (!context.hasFunction(name))
    {
        std::cerr << "Calling undefined Function" << std::endl;
        return std::make_shared<NullValue>();
    }

    // TODO: Set params = args in SubContext, then evaluate function
    const auto& params = context.getFunction(name).first;

    if (params.size() != args.size())
    {
        std::cerr << "Calling Function with wrong number of arguments" << std::endl;
        return std::make_shared<NullValue>();
    }

    auto func = context.getFunction(name).second;
    for (uint i = 0; i < params.size(); ++i)
    {
        context.setVariable(params[i], args[i]);
    }
    return func->evaluate(context);
}

void FunctionCallNode::print(uint depth) const
{

};

//==================
// Empty Node
//==================

EmptyNode::EmptyNode()
{}

const std::shared_ptr<Value> EmptyNode::evaluate(Scope& context) const
{
    return std::make_shared<NullValue>();
}

void EmptyNode::print(uint depth) const
{}

}
