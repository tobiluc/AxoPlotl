#ifndef TREE_H
#define TREE_H

#include <cassert>
#include <iostream>
#include "tokens.h"

namespace AxPl::Parsing
{

struct Value
{
    virtual ~Value() = default;
    virtual void print() const = 0;
};

struct ScalarValue : public Value
{
    double value;

    explicit ScalarValue(double val) : value(val) {}

    inline double getValue() const {return value;}

    inline void print() const override {std::cout << value;}
};

class PointValue : public Value {
public:
    double x, y, z;

    PointValue(double x, double y, double z) : x(x), y(y), z(z) {}

    template<typename VecT>
    inline VecT getValue() {return VecT{x, y, z};}

    inline void print() const override {std::cout << "Point(" << x << ", " << y << ", " << z <<  ")";}

    inline double& operator[](int i)
    {
        assert(i >= 0 && i <= 2);
        return (i==0)? x : (i==1)? y : z;
    }

    inline const double operator[](int i) const
    {
        assert(i >= 0 && i <= 2);
        return (i==0)? x : (i==1)? y : z;
    }
};

struct ListValue : public Value
{
    std::vector<std::unique_ptr<Value>> values;

    explicit ListValue(std::vector<std::unique_ptr<Value>>& values)
        : values(std::move(values))
    {
    }

    inline void print() const override
    {
        std::cout << "List(" << std::endl;
        for (uint i = 0; i < values.size(); ++i)
        {
            values[i]->print();
            if (i < values.size()-1) std::cout << ", "; else std::cout << ")";
        }
    }

    inline const std::vector<std::unique_ptr<Value>>& getValue() const {return values;}
};

struct ASTNode
{
    virtual ~ASTNode() = default;
    virtual std::unique_ptr<Value> evaluate() const = 0;
};

struct ScalarNode : public ASTNode
{
    double value;

    explicit ScalarNode(double val) : value(val) {}

    inline std::unique_ptr<Value> evaluate() const override
    {
        return std::make_unique<ScalarValue>(value);
    }
};

struct PointNode : public ASTNode
{
    double x, y, z;

    explicit PointNode(double x, double y, double z) :
        x(x), y(y), z(z)
    {}

    inline std::unique_ptr<Value> evaluate() const override
    {
        return std::make_unique<PointValue>(x, y, z);
    }
};

struct ListNode : public ASTNode
{
    std::vector<std::unique_ptr<ASTNode>> children;

    explicit ListNode(std::vector<std::unique_ptr<ASTNode>>& children) :
        children(std::move(children))
    {}

    inline std::unique_ptr<Value> evaluate() const override
    {
        std::vector<std::unique_ptr<Value>> values;
        values.reserve(children.size());
        for (auto& child : children) values.push_back(child->evaluate());
        return std::make_unique<ListValue>(values);
    }
};

struct VariableNode : public ASTNode
{
    std::string name;

    explicit VariableNode(std::string n) : name(std::move(n)) {}

    std::unique_ptr<Value> evaluate() const override
    {
        // TODO: Evaluate Variable
        throw std::runtime_error("Variable evaluation not implemented!");
    }
};

struct BinaryOpNode : public ASTNode
{
    Token::Type op;
    std::unique_ptr<ASTNode> left, right;

    BinaryOpNode(Token::Type op, std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right) :
        op(op),
        left(std::move(left)),
        right(std::move(right))
    {}

    inline std::unique_ptr<Value> evaluate() const override
    {
        auto leftValue = left->evaluate();
        auto rightValue = right->evaluate();

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
                return std::make_unique<ScalarValue>(lval + rval);
            case Token::Type::MINUS:
                return std::make_unique<ScalarValue>(lval - rval);
            case Token::Type::TIMES:
                return std::make_unique<ScalarValue>(lval * rval);
            case Token::Type::DIV:
                if (rval == 0) throw std::runtime_error("Division by zero!");
                return std::make_unique<ScalarValue>(lval / rval);
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
                return std::make_unique<PointValue>(
                    lval[0] + rval[0],
                    lval[1] + rval[1],
                    lval[2] + rval[2]
                );
            case Token::Type::MINUS:
                return std::make_unique<PointValue>(
                    lval[0] - rval[0],
                    lval[1] - rval[1],
                    lval[2] - rval[2]
                );
            case Token::Type::TIMES:
                return std::make_unique<PointValue>(
                    lval[0] * rval[0],
                    lval[1] * rval[1],
                    lval[2] * rval[2]
                );
            case Token::Type::DIV:
                if (!rval[0] || !rval[1] || !rval[2]) throw std::runtime_error("Division by zero!");
                return std::make_unique<PointValue>(
                    lval[0] / rval[0],
                    lval[1] / rval[1],
                    lval[2] / rval[2]
                );
            case Token::Type::PERCENT:
                return std::make_unique<PointValue>(
                    lval[1] * rval[2] - lval[2] * rval[1],
                    lval[2] * rval[0] - lval[0] * rval[2],
                    lval[0] * rval[1] - lval[1] * rval[0]
                );
            default:
                throw std::runtime_error("Unknown operator for vectors");
            }
        }

        throw std::runtime_error("Incompatible types for operator");
    }
};

struct UnaryOpNode : public ASTNode
{
    Token::Type op;
    std::unique_ptr<ASTNode> child;

    UnaryOpNode(Token::Type op, std::unique_ptr<ASTNode> child) :
        op(op),
        child(std::move(child))
    {}

    inline std::unique_ptr<Value> evaluate() const override
    {
        auto value = child->evaluate();

        // Scalar Operations
        auto scalar = dynamic_cast<ScalarValue*>(value.get());
        if (scalar)
        {
            double val = scalar->getValue();
            switch (op)
            {
            case Token::Type::PLUS:
                return std::make_unique<ScalarValue>(val);
            case Token::Type::MINUS:
                return std::make_unique<ScalarValue>(-val);
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
                return std::make_unique<PointValue>(
                    val[0],
                    val[1],
                    val[2]
                );
            case Token::Type::MINUS:
                return std::make_unique<PointValue>(
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
};

struct AssignOpNode : public ASTNode
{
    std::string varName;
    std::unique_ptr<ASTNode> right;

    AssignOpNode(std::string varName, std::unique_ptr<ASTNode> right) :
        varName(varName),
        right(std::move(right))
    {}

    inline std::unique_ptr<Value> evaluate() const override
    {
        // TODO: ASSIGN
        return right->evaluate();
    }
};

struct EmptyNode : public ASTNode
{
    EmptyNode() {}

    inline std::unique_ptr<Value> evaluate() const override
    {
        return nullptr;
    }
};

}

#endif // TREE_H
