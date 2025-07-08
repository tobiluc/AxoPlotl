#pragma once

#include <nlohmann/json.hpp>
#include "AxoPlotl/geometry/glm.h"
#include "nlohmann/json_fwd.hpp"
#include "AxoPlotl/utils/Typedefs.h"
#include <iostream>
#include <string>
#include <ostream>
#include <fstream>

namespace AxoPlotl::IO
{

template<typename T>
bool deserialize(const std::string& filename, T& obj) {
    std::ifstream file(filename);
    if (!file) {return false;}
    try {
        nlohmann::json j;
        file >> j;
        obj = j.get<T>();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

template<typename T>
bool serialize(const std::string& filename, const T& obj) {
    std::ofstream file(filename);
    if (!file) {return false;}

    try {
        file << nlohmann::json(obj).dump(4);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
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
