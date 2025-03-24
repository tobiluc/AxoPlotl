#ifndef TRIANGLESRENDERBATCH_H
#define TRIANGLESRENDERBATCH_H

#include "RenderBatch.h"

namespace MV {

class TrianglesRenderBatch : public RenderBatch<Triangle>
{
public:
    TrianglesRenderBatch(size_t max_num_triangles);

    TrianglesRenderBatch(TetrahedralMesh& mesh);

    void initFromMesh(TetrahedralMesh& mesh);

    void render();

    void renderPicking(const glm::mat4x4& mvp, const uint batch_idx);
};

}

#endif // TRIANGLESRENDERBATCH_H
