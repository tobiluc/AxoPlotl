#ifndef TETMESHRENDERER_H
#define TETMESHRENDERER_H

#include "LineBatch.h"
#include "TriangleBatch.h"

namespace MV
{

class TetMeshRenderer
{
public:
    TetMeshRenderer(TetrahedralMesh& mesh);

    void render(const glm::mat4x4& v, const glm::mat4x4& p);

private:
    TetrahedralMesh& mesh;
    TriangleBatch cellsBatch;
    TriangleBatch facesBatch;
    TriangleBatch boundaryFacesBatch;
    LineBatch edgesBatch;

    Shader triangleShader;
    Shader lineShader;

    void createCellsBatch(float scale);

    void createFacesBatch();

    void createEdgesBatch();
};

}

#endif // TETMESHRENDERER_H
