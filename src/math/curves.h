#ifndef CURVES_H
#define CURVES_H

#include <functional>

namespace MV
{

template<typename IN, typename OUT>
class ExplicitCurve
{
private:
    std::function<OUT(IN)> f;

public:
    ExplicitCurve(std::function<OUT(IN)> f) :
        f(f)
    {

    }

    inline OUT compute(const IN& x) const
    {
        return f(x);
    }

    inline std::vector<OUT> compute(const std::vector<IN>& xs) const
    {
        std::vector<OUT> ys;
        ys.reserve(xs.size());
        for (const IN& x : xs) ys.push_back(compute(x));
        return ys;
    }
};

class Sin : public ExplicitCurve<float, float>
{
public:
    Sin() : ExplicitCurve([](float x) {return std::sin(x);})
    {}
};

}

#endif // CURVES_H
