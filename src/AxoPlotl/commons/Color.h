#ifndef COLOR_H
#define COLOR_H

#include <array>
#include <functional>

namespace AxoPlotl
{

struct Color
{
    std::array<float, 3> rgb;

    Color(float r, float g, float b) :
        rgb({r, g, b})
    {}

    inline const float operator[](int i) const
    {
        return rgb[i];
    }

    inline float& operator[](int i)
    {
        return rgb[i];
    }

    inline const float* data() const
    {
        return rgb.data();
    }

    inline Color mix(const Color& color, float t = 0.5) const
    {
        return Color(
            t*rgb[0] + (1.0-t)*color.rgb[0],
            t*rgb[1] + (1.0-t)*color.rgb[1],
            t*rgb[2] + (1.0-t)*color.rgb[2]
        );
    }

    static const Color RED;
    static const Color BLUE;
    static const Color GREEN;
    static const Color BLACK;
    static const Color LIGHTGRAY;
    static const Color WHITE;

    static const std::array<Color, 6> COLORS;

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
