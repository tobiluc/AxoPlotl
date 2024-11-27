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
    TetMeshRenderer(TetrahedralMesh& mesh)  :
        mesh(mesh),
        cellScale(0.9f),
        outlineWidth(2),
        outlineColor({0,0,0}),
        tetCellsBatch(mesh),
        facesBatch(mesh),
        edgesBatch(mesh),
        edgesShader("../res/shaders/edges.vert", "../res/shaders/edges.frag"),
        facesShader("../res/shaders/faces.vert", "../res/shaders/faces.geom", "../res/shaders/faces.frag"),
        tetCellsShader("../res/shaders/cells.vert", "../res/shaders/cells.geom", "../res/shaders/cells.frag"),
        light({Vec3f(0.5,0.5,0.5), Vec3f(0.3,0.3,0.3), Vec3f(0.2,0.2,0.2)})
    {}

    inline void setMesh(TetrahedralMesh& mesh)
    {
        //if (&this->mesh == &mesh) return;
        this->mesh = mesh;
        tetCellsBatch.initFromMesh(mesh);
        facesBatch.initFromMesh(mesh);
        edgesBatch.initFromMesh(mesh);
    }

    void render();

    bool showCells = true, showFaces = false, showEdges = false;
    Light light;
    float cellScale;
    float outlineWidth;
    Color outlineColor;
    bool useColorOverride = false;
    Color colorOverride;

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
