#ifndef TREE_H
#define TREE_H

#include "tokens.h"
#include "values.h"

namespace AxoPlotl
{
class Scope;
}

namespace AxoPlotl::Parsing
{

struct ASTNode
{
    virtual ~ASTNode() = default;
    virtual const std::shared_ptr<Value> evaluate(Scope& context) const = 0;

    virtual void print(uint depth = 0) const = 0;
};

struct ScalarNode : public ASTNode
{
    double value;

    explicit ScalarNode(double val);

    const std::shared_ptr<Value> evaluate(Scope& context) const override;

    void print(uint depth) const override;
};

struct PointNode : public ASTNode
{
    double x, y, z;

    explicit PointNode(double x, double y, double z);

    const std::shared_ptr<Value> evaluate(Scope& context) const override;

    void print(uint depth) const override;
};

struct ListNode : public ASTNode
{
    std::vector<std::shared_ptr<ASTNode>> children;

    explicit ListNode(const std::vector<std::shared_ptr<ASTNode>>& children);

    const std::shared_ptr<Value> evaluate(Scope& context) const override;

    void print(uint depth) const override;
};

struct VariableNode : public ASTNode
{
    std::string name;

    explicit VariableNode(const std::string& name);

    const std::shared_ptr<Value> evaluate(Scope& context) const override;

    void print(uint depth) const override;
};

struct BinaryOpNode : public ASTNode
{
    Token::Type op;
    std::shared_ptr<ASTNode> left;
    std::shared_ptr<ASTNode> right;

    explicit BinaryOpNode(Token::Type op, std::shared_ptr<ASTNode> left, std::shared_ptr<ASTNode> right);

    const std::shared_ptr<Value> evaluate(Scope& context) const override;

    void print(uint depth) const override;
};

struct UnaryOpNode : public ASTNode
{
    Token::Type op;
    std::shared_ptr<ASTNode> child;

    explicit UnaryOpNode(Token::Type op, std::shared_ptr<ASTNode> child);

    const std::shared_ptr<Value> evaluate(Scope& context) const override;

    void print(uint depth) const override;
};

struct AssignNode : public ASTNode
{
    std::string name;
    std::shared_ptr<ASTNode> right;

    explicit AssignNode(const std::string& name, std::shared_ptr<ASTNode> right);

    const std::shared_ptr<Value> evaluate(Scope& context) const override;

    void print(uint depth) const override;
};

struct FunctionAssignNode : public ASTNode
{
    std::string name;
    std::vector<std::string> params;
    std::shared_ptr<ASTNode> func;

    explicit FunctionAssignNode(const std::string& name, const std::vector<std::string>& params, std::shared_ptr<ASTNode> func);

    const std::shared_ptr<Value> evaluate(Scope& context) const override;

    void print(uint depth) const override;
};

struct FunctionCallNode : public ASTNode
{
    std::string name;
    std::vector<std::shared_ptr<ASTNode>> args;

    explicit FunctionCallNode(const std::string& name, const std::vector<std::shared_ptr<ASTNode>>& args);

    const std::shared_ptr<Value> evaluate(Scope& context) const override;

    void print(uint depth) const override;
};

struct EmptyNode : public ASTNode
{
    explicit EmptyNode();

    const std::shared_ptr<Value> evaluate(Scope& context) const override;

    void print(uint depth) const override;
};

}

#endif // TREE_H
