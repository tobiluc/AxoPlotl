#include "Color.h"

namespace MV
{

const Color Color::RED   = Color(1.0f, 0.0f, 0.0f);
const Color Color::BLUE  = Color(0.0f, 0.0f, 1.0f);
const Color Color::GREEN = Color(0.0f, 1.0f, 0.0f);
const Color Color::BLACK = Color(0.0f, 0.0f, 0.0f);
const Color Color::WHITE = Color(1.0f, 1.0f, 1.0f);

const std::array<Color, 5> Color::COLORS = {
    RED, BLUE, GREEN, BLACK, WHITE
};

}
