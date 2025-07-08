#ifndef TRIANGLESRENDERBATCH_H
#define TRIANGLESRENDERBATCH_H

#include "AxoPlotl/geometry/ovm.h"
#include "AxoPlotl/rendering/Primitives.h"
#include "RenderBatch.h"

namespace AxoPlotl::Rendering {

class TrianglesRenderBatch : public RenderBatch<Triangle>
{
public:
    TrianglesRenderBatch(size_t max_num_triangles);

    TrianglesRenderBatch(TetrahedralMesh& mesh);

    void initFromMesh(TetrahedralMesh& mesh);

    void initFromMesh(HexahedralMesh& mesh);

    void render();

    void renderPicking(const glm::mat4x4& mvp, const uint object_index);
};

}

#endif // TRIANGLESRENDERBATCH_H
