#include "Color.h"
#include <random>

namespace AxoPlotl
{

const Color Color::RED   = Color(1.0f, 0.0f, 0.0f);
const Color Color::BLUE  = Color(0.0f, 0.0f, 1.0f);
const Color Color::GREEN = Color(0.0f, 1.0f, 0.0f);
const Color Color::BLACK = Color(0.0f, 0.0f, 0.0f);
const Color Color::WHITE = Color(1.0f, 1.0f, 1.0f);

const std::array<Color, 5> Color::COLORS = {
    RED, BLUE, GREEN, BLACK, WHITE
};

Color Color::random()
{
    std::mt19937_64 rng;
    uint64_t seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::seed_seq ss{uint32_t(seed & 0xffffffff), uint32_t(seed>>32)};
    rng.seed(ss);
    std::uniform_real_distribution<float> unif(0, 1);

    return Color(unif(rng), unif(rng), unif(rng));
}

}
