#ifndef GEOMETRYOBJECT_H
#define GEOMETRYOBJECT_H

#include <cstdint>

namespace MV
{

class GeometryObject
{
public:
    GeometryObject();

    uint32_t renderToBatch();
};

}

#endif // GEOMETRYOBJECT_H
