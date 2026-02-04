#pragma once

#include <nlohmann/json.hpp>
#include "AxoPlotl/geometry/glm.h"
#include "nlohmann/json_fwd.hpp"
#include <iostream>
#include <string>
#include <ostream>
#include <fstream>

namespace AxoPlotl::IO
{

inline bool load_json(const std::filesystem::path& _path, nlohmann::json& _j)
{
    std::ifstream file(_path);
    if (!file) {return false;}
    try {
        file >> _j;
        return true;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
}

inline bool save_json(const std::filesystem::path& _path, const nlohmann::json& _j)
{
    std::ofstream file(_path);
    if (!file) {return false;}

    try {
        file << _j.dump(4);
        return true;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
}

} // !namespace AxoPlotl::IO

namespace nlohmann {

inline void to_json(json& j, const AxoPlotl::Vec3f& v) {
    j = json{{"x", v.x}, {"y", v.y}, {"z", v.z}};
}

inline void from_json(const json& j, AxoPlotl::Vec3f& v) {
    j.at("x").get_to(v.x);
    j.at("y").get_to(v.y);
    j.at("z").get_to(v.z);
}

} // !namespace nlohmann
