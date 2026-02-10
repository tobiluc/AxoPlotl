#pragma once

#include <array>
namespace AxoPlotl
{

struct AABB {
    double x0, x1, y0, y1, z0, z1;

    inline std::array<double,3> center() const {
        return {
            0.5 * (x0 + x1),
            0.5 * (y0 + y1),
            0.5 * (z0 + z1)
        };
    }

    inline std::array<double,3> size() const {
        return {x1-x0,y1-y0,z1-z0};
    }

    inline double diagonal() const {
        const auto s = size();
        return std::sqrt(s[0]*s[0]+s[1]*s[1]+s[2]*s[2]);
    }

    template<typename Vec3T>
    inline void compute(const std::vector<Vec3T>& pts) {
        x0 = y0 = z0 = +std::numeric_limits<double>::infinity();
        x1 = y1 = z1 = -std::numeric_limits<double>::infinity();
        for (unsigned int i = 0; i < pts.size(); ++i) {
            const Vec3T& p = pts[i];
            if (p[0] < x0) x0 = p[0];
            if (p[0] > x1) x1 = p[0];
            if (p[1] < y0) y0 = p[1];
            if (p[1] > y1) y1 = p[1];
            if (p[2] < z0) z0 = p[2];
            if (p[2] > z1) z1 = p[2];
        }
    }

    template<typename Vec3T>
    inline std::array<Vec3T,8> corners() const {
        return {
            Vec3T(x0, y0, z0),
            Vec3T(x0, y0, z1),
            Vec3T(x1, y0, z1),
            Vec3T(x1, y0, z0),

            Vec3T(x0, y1, z0),
            Vec3T(x1, y1, z0),
            Vec3T(x1, y1, z1),
            Vec3T(x0, y1, z1)
        };
    }
};

}
