#ifndef MESHRENDERER_H
#define MESHRENDERER_H

#include "../utils/Typedefs.h"
#include "Renderer.h"

namespace MV
{
using RenderSettings = Renderer::RenderSettings;

class MeshViewer;

class MeshRenderer
{
public:
    virtual void render(MeshViewer& mv) = 0;

    virtual void renderPicking(const glm::mat4x4& mvp, const uint object_id) = 0;

    virtual void deleteBuffers() = 0;

    RenderSettings settings;
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
