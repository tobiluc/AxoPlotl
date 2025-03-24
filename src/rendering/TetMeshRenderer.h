#ifndef TETMESHRENDERER_H
#define TETMESHRENDERER_H

#include <glad/glad.h>
#include "LinesRenderBatch.h"
#include "MeshRenderer.h"
#include "PointsRenderBatch.h"
#include "RenderBatchTetCells.h"
#include "RenderBatchTetFaces.h"
#include "../commons/Camera.h"
#include "TrianglesRenderBatch.h"

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

    ~TetMeshRenderer()
    {
    }

    inline void deleteBuffers() override
    {
        tetCellsBatch.deleteBuffers();
        facesBatch.deleteBuffers();
        edgesBatch.deleteBuffers();
        verticesBatch.deleteBuffers();
    }

    inline void setMesh(TetrahedralMesh& mesh)
    {
        this->mesh = mesh;
        tetCellsBatch.initFromMesh(mesh);
        facesBatch.initFromMesh(mesh);
        edgesBatch.initFromMesh(mesh);
        verticesBatch.initFromMesh(mesh);
    }

    void render(MeshViewer& mv) override;

    void renderPicking(const glm::mat4x4& mvp, const uint object_id) override;

    //MeshRenderSettings settings;

private:
    Camera& camera;

    RenderBatchTetCells tetCellsBatch;
    //RenderBatchTetFaces facesBatch;
    //RenderBatchEdges<TetrahedralMesh> edgesBatch;
    TrianglesRenderBatch facesBatch;
    LinesRenderBatch edgesBatch;
    PointsRenderBatch verticesBatch;
    //RenderBatchVertices<TetrahedralMesh> verticesBatch;
};

}

#endif // TETMESHRENDERER_H
