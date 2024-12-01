#ifndef HEXMESHRENDERER_H
#define HEXMESHRENDERER_H

#include "../utils/Typedefs.h"
#include "MeshRenderer.h"
#include "RenderBatchEdges.h"
#include "RenderBatchHexCells.h"
#include "RenderBatchVertices.h"
#include "../commons/Camera.h"

namespace MV
{
class HexMeshRenderer : public MeshRendererT<HexahedralMesh>
{
public:
    HexMeshRenderer(HexahedralMesh& mesh, Camera& camera) :
        MeshRendererT(mesh),

        camera(camera),

        hexCellsBatch(mesh),
        edgesBatch(mesh),
        verticesBatch(mesh)
    {

    }

    void render() override;

    inline void setMesh(HexahedralMesh& mesh)
    {
        this->mesh = mesh;
        hexCellsBatch.initFromMesh(mesh);
    }
private:
    //HexahedralMesh& mesh;
    Camera& camera;

    RenderBatchHexCells hexCellsBatch;
    RenderBatchEdges<HexahedralMesh> edgesBatch;
    RenderBatchVertices<HexahedralMesh> verticesBatch;
};
}

#endif // HEXMESHRENDERER_H
