#ifndef COLOR_H
#define COLOR_H

#include <array>

namespace MV
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

    static const Color RED;
    static const Color BLUE;
    static const Color GREEN;
    static const Color BLACK;
    static const Color WHITE;

    static const std::array<Color, 5> COLORS;

};

}

#endif // COLOR_H
