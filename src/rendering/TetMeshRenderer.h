#ifndef TETMESHRENDERER_H
#define TETMESHRENDERER_H

#include "RenderBatchEdges.h"
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
    float outlineWidth;
    Color outlineColor;

private:
    TetrahedralMesh& mesh;

    RenderBatchTetCells tetCellsBatch;
    Shader tetCellsShader;
    //Shader tetCellsOutlineShader;

    RenderBatchTetFaces facesBatch;
    Shader facesShader;

    RenderBatchEdges edgesBatch;
    Shader edgesShader;
};

}

#endif // TETMESHRENDERER_H
