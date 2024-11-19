#pragma once

#include "Typedefs.h"

namespace MV
{

    inline std::string toLower(const std::string& s)
    {
        std::string r = s;
        std::transform(r.begin(), r.end(), r.begin(),[](unsigned char c){return std::tolower(c);});
        return r;
    }

    inline std::pair<std::string,std::string> splitFilenameExtension(const std::string& filename)
    {
        auto i = filename.find_last_of(".");
        return std::make_pair(filename.substr(0,i), filename.substr(i));
    }

    
} // namespace MV
