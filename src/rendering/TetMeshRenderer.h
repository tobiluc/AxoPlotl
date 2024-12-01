#ifndef TETMESHRENDERER_H
#define TETMESHRENDERER_H

#include <glad/glad.h>
#include "MeshRenderer.h"
#include "RenderBatchEdges.h"
#include "RenderBatchTetCells.h"
#include "RenderBatchTetFaces.h"
#include "RenderBatchVertices.h"
#include "../commons/Camera.h"

namespace MV
{

class TetMeshRenderer : public MeshRendererT<TetrahedralMesh>
{
public:
    TetMeshRenderer(TetrahedralMesh& mesh, Camera& camera)  :
        MeshRendererT(mesh),

        camera(camera),

        tetCellsBatch(mesh),
        facesBatch(mesh),
        edgesBatch(mesh),
        verticesBatch(mesh)
    {}

    // Disable copy constructor and assignment operator
    //TetMeshRenderer(const TetMeshRenderer&) = delete;
    //TetMeshRenderer& operator=(const TetMeshRenderer&) = delete;

    inline void setMesh(TetrahedralMesh& mesh)
    {
        this->mesh = mesh;
        tetCellsBatch.initFromMesh(mesh);
        facesBatch.initFromMesh(mesh);
        edgesBatch.initFromMesh(mesh);
        verticesBatch.initFromMesh(mesh);
    }

    void render() override;

    //MeshRenderSettings settings;

private:
    Camera& camera;

    RenderBatchTetCells tetCellsBatch;
    RenderBatchTetFaces facesBatch;
    RenderBatchEdges<TetrahedralMesh> edgesBatch;
    RenderBatchVertices<TetrahedralMesh> verticesBatch;
};

}

#endif // TETMESHRENDERER_H
