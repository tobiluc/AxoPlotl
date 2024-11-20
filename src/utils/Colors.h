#pragma once

namespace MV
{
    struct Color {float r, g, b, a;};

    constexpr Color RED = {1, 0, 0, 1};
    constexpr Color GREEN = {0, 1, 0, 1};
    constexpr Color BLUE = {0, 0, 1, 1};
    constexpr Color WHITE = {1, 1, 1, 1};
    constexpr Color BLACK = {0, 0, 0, 1};
}