#ifndef TETMESHRENDERER_H
#define TETMESHRENDERER_H

#include "LineBatch.h"
#include "RenderBatchTetCells.h"
#include "RenderBatchTetFaces.h"

namespace MV
{

class TetMeshRenderer
{
public:
    TetMeshRenderer(TetrahedralMesh& mesh);

    void render(const glm::mat4x4& v, const glm::mat4x4& p);

    bool showCells = true, showFaces = false, showEdges = false;
    Light light;
    float cellScale;

private:
    TetrahedralMesh& mesh;
    LineBatch edgesBatch;

    RenderBatchTetCells tetCellsBatch;
    Shader tetCellsShader;

    RenderBatchTetFaces facesBatch;
    Shader facesShader;

    Shader lineShader;

    void createEdgesBatch();
};

}

#endif // TETMESHRENDERER_H
