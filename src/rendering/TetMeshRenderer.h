#ifndef TETMESHRENDERER_H
#define TETMESHRENDERER_H

#include "TriangleBatch.h"

namespace MV
{

class TetMeshRenderer
{
public:
    TetMeshRenderer(TetrahedralMesh& mesh, Shader& shader);

    void render();

private:
    TetrahedralMesh& mesh;
    TriangleBatch cellsBatch;
    TriangleBatch facesBatch;
    TriangleBatch boundaryFacesBatch;
    MeshRenderMode meshMode;
};

}

#endif // TETMESHRENDERER_H
