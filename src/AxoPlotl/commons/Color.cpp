#include "Color.h"
#include <random>

namespace AxoPlotl
{

Vec3f random_color()
{
    std::mt19937_64 rng;
    uint64_t seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::seed_seq ss{uint32_t(seed & 0xffffffff), uint32_t(seed>>32)};
    rng.seed(ss);
    std::uniform_real_distribution<float> unif(0, 1);

    return Vec3f(unif(rng), unif(rng), unif(rng));
}

}
