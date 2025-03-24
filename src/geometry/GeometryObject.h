#ifndef GEOMETRYOBJECT_H
#define GEOMETRYOBJECT_H

#include "../rendering/Renderer.h"

namespace MV
{

class GeometryObject
{
public:
    GeometryObject();

    std::string name;
    Renderer::GeometryLocation location;
};

}

#endif // GEOMETRYOBJECT_H
