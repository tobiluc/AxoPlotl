#ifndef COLOR_H
#define COLOR_H

#include <array>
#include <functional>

namespace AxoPlotl
{

struct Color
{
    std::array<float,4> rgba;

    Color(float r, float g, float b) :
        rgba({r, g, b, 1})
    {}

    Color(float r, float g, float b, float a) :
        rgba({r, g, b, a})
    {}

    inline const float operator[](int i) const
    {
        return rgba[i];
    }

    inline float& operator[](int i)
    {
        return rgba[i];
    }

    // inline const float* data() const
    // {
    //     return rgba.data();
    // }

    inline Color mix(const Color& color, float t = 0.5) const
    {
        return Color(
            t*rgba[0] + (1.0-t)*color.rgba[0],
            t*rgba[1] + (1.0-t)*color.rgba[1],
            t*rgba[2] + (1.0-t)*color.rgba[2],
            t*rgba[3] + (1.0-t)*color.rgba[3]
        );
    }

    static const Color RED;
    static const Color BLUE;
    static const Color GREEN;
    static const Color BLACK;
    static const Color LIGHTGRAY;
    static const Color DARKGRAY;
    static const Color WHITE;

    static Color random();

};

class ColorFunction1f
{
private:
    std::function<Color(float)> f_;

public:
    ColorFunction1f(const std::function<Color(float)>& _f) : f_(_f)
    {};

    ColorFunction1f(const Color& color) : ColorFunction1f([&](float u) {return color;})
    {}

    inline Color operator()(float u) const {
        return f_(u);
    }
};

class ColorFunction2f
{
private:
    std::function<Color(float,float)> f_;

public:
    ColorFunction2f(const std::function<Color(float,float)>& _f) : f_(_f)
    {};

    ColorFunction2f(const Color& color) : ColorFunction2f([&](float u,float v) {return color;})
    {}

    inline Color operator()(float u, float v) const {
        return f_(u, v);
    }
};

struct Light {Color ambient, diffuse, specular;};

inline Color getColorOnSphere(float x, float y, float z) {
    return Color(
        0.5 * (x + 1),
        0.5 * (y + 1),
        0.5 * (z + 1)
        );
}

} // namespace AxoPlotl

#endif // COLOR_H
