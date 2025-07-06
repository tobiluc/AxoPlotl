#ifndef VALUES_H
#define VALUES_H

#include <cassert>
#include <iostream>

namespace AxoPlotl::Parsing
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

    inline void print() const override {std::cout << "(" << x << ", " << y << ", " << z <<  ")";}

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
    std::vector<std::shared_ptr<Value>> values;

    explicit ListValue(const std::vector<std::shared_ptr<Value>>& values)
        : values(values)
    {
    }

    inline void print() const override
    {
        std::cout << "List[";
        for (uint i = 0; i < values.size(); ++i)
        {
            values[i]->print();
            if (i < values.size()-1) std::cout << ", "; else std::cout << "]";
        }
    }

    inline const std::vector<std::shared_ptr<Value>>& getValue() const {return values;}
};

struct NullValue : public Value
{
    explicit NullValue() {}

    inline void print() const override {std::cout << "NULL";}

    inline const double getValue() const {return 0;}
};

}

#endif // VALUES_H
