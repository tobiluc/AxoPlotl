#ifndef TREE_H
#define TREE_H

#include "tokens.h"

namespace AxoPlotl
{
class Scope;
}

namespace AxoPlotl::Parsing
{

struct Vec3d {
    double x, y, z;

    Vec3d(double x, double y, double z) : x(x), y(y), z(z) {}

    inline friend Vec3d operator+(const Vec3d& v) {return v;}
    inline friend Vec3d operator-(const Vec3d& v) {return Vec3d(-v.x,-v.y,-v.z);}
    inline friend Vec3d operator+(const Vec3d& v, const Vec3d& w) {return Vec3d(v.x+w.x,v.y+w.y,v.z+w.z);}
    inline friend Vec3d operator-(const Vec3d& v, const Vec3d& w) {return Vec3d(v.x-w.x,v.y-w.y,v.z-w.z);}
    inline friend double operator*(const Vec3d& v, const Vec3d& w) {return v.x*w.x+v.y*w.y+v.z*w.z;}
    inline friend Vec3d operator*(const Vec3d& v, double s) {return Vec3d(v.x*s,v.y*s,v.z*s);}
    inline friend Vec3d operator*(double s, const Vec3d& v) {return Vec3d(v.x*s,v.y*s,v.z*s);}
    inline friend Vec3d operator/(const Vec3d& v, double d) {return Vec3d(v.x/d,v.y/d,v.z/d);}
    inline friend Vec3d operator/(const Vec3d& v, const Vec3d& w) {return Vec3d(v.x/w.x,v.y/w.y,v.z/w.z);}

    inline friend std::ostream& operator<<(std::ostream& os, const Vec3d& v) {
        return os << v.x << " " << v.y << " " << v.z;
    }
};

struct EvalValue;
using EvalValueList = std::vector<EvalValue>;
using EvalType = std::variant<double, Vec3d, EvalValueList>;
using EvalFunction = std::function<EvalValue(Scope&)>;

struct EvalValue {
    EvalType val;

    EvalValue() = default;
    template<typename T>
    EvalValue(T v) : val(std::move(v)) {}

    void print() const {
        std::visit([](auto&& v) {
            using T = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<T, double>)
                std::cout << "Scalar: " << v << "\n";
            else if constexpr (std::is_same_v<T, Vec3d>)
                std::cout << "Vec3: (" << v << ")\n";
            else
                std::cout << "Unsupported type\n";
        }, val);
    }

    template<typename RT>
    RT value() const {
        return std::visit([](auto&& v) {
            using T = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<T, RT>) return v;
            return RT();
        }, val);
    }
};

struct ASTNode
{
    virtual ~ASTNode() = default;

    virtual const EvalValue evaluate(Scope& context) const = 0;

    virtual EvalFunction compile() const = 0;

    virtual void print(uint depth = 0) const = 0;
};

struct ScalarNode : public ASTNode
{
    double value;

    explicit ScalarNode(double val);

    const EvalValue evaluate(Scope& context) const override;

    EvalFunction compile() const override;

    void print(uint depth) const override;
};

struct PointNode : public ASTNode
{
    Vec3d p;

    explicit PointNode(double x, double y, double z);

    const EvalValue evaluate(Scope& context) const override;

    EvalFunction compile() const override;

    void print(uint depth) const override;
};

struct ListNode : public ASTNode
{
    std::vector<std::shared_ptr<ASTNode>> children;

    explicit ListNode(const std::vector<std::shared_ptr<ASTNode>>& children);

    const EvalValue evaluate(Scope& context) const override;

    EvalFunction compile() const override;

    void print(uint depth) const override;
};

struct VariableNode : public ASTNode
{
    std::string name;

    explicit VariableNode(const std::string& name);

    const EvalValue evaluate(Scope& context) const override;

    EvalFunction compile() const override;

    void print(uint depth) const override;
};

struct BinaryOpNode : public ASTNode
{
    Token::Type op;
    std::shared_ptr<ASTNode> left;
    std::shared_ptr<ASTNode> right;

    explicit BinaryOpNode(Token::Type op, std::shared_ptr<ASTNode> left, std::shared_ptr<ASTNode> right);

    const EvalValue evaluate(Scope& context) const override;

    EvalFunction compile() const override;

    void print(uint depth) const override;
};

struct UnaryOpNode : public ASTNode
{
    Token::Type op;
    std::shared_ptr<ASTNode> child;

    explicit UnaryOpNode(Token::Type op, std::shared_ptr<ASTNode> child);

    const EvalValue evaluate(Scope& context) const override;

    EvalFunction compile() const override;

    void print(uint depth) const override;
};

// struct AssignNode : public ASTNode
// {
//     std::string name;
//     std::shared_ptr<ASTNode> right;

//     explicit AssignNode(const std::string& name, std::shared_ptr<ASTNode> right);

//     const EvalValue evaluate(Scope& context) const override;

//     void print(uint depth) const override;
// };

// struct FunctionAssignNode : public ASTNode
// {
//     std::string name;
//     std::vector<std::string> params;
//     std::shared_ptr<ASTNode> func;

//     explicit FunctionAssignNode(const std::string& name, const std::vector<std::string>& params, std::shared_ptr<ASTNode> func);

//     const EvalValue evaluate(Scope& context) const override;

//     void print(uint depth) const override;
// };

struct FunctionCallNode : public ASTNode
{
    std::string name;
    std::vector<std::shared_ptr<ASTNode>> args;

    explicit FunctionCallNode(const std::string& name, const std::vector<std::shared_ptr<ASTNode>>& args);

    const EvalValue evaluate(Scope& context) const override;

    EvalFunction compile() const override;

    void print(uint depth) const override;
};

struct EmptyNode : public ASTNode
{
    explicit EmptyNode();

    const EvalValue evaluate(Scope& context) const override;

    EvalFunction compile() const override;

    void print(uint depth) const override;
};

}

#endif // TREE_H
