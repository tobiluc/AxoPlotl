#ifndef MESHRENDERER_H
#define MESHRENDERER_H

#include "../utils/Typedefs.h"

namespace MV
{

class MeshRenderer
{
public:
    virtual void render() = 0;

    virtual void renderPicking() = 0;

    virtual void deleteBuffers() = 0;

    MeshRenderSettings settings;
};

template<typename MeshT>
class MeshRendererT : public MeshRenderer
{
public:
    MeshRendererT(MeshT& mesh) :
        MeshRenderer(),
        mesh(mesh)
    {}

protected:
    MeshT& mesh;
};

}

#endif // MESHRENDERER_H
