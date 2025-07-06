#ifndef LINESRENDERBATCH_H
#define LINESRENDERBATCH_H

#include "GLBuffers.h"
#include "PointsRenderBatch.h"
#include "RenderBatch.h"

namespace AxoPlotl::Rendering
{

class LinesRenderBatch : public RenderBatch<Line>
{
public:
    LinesRenderBatch(size_t max_num_lines);

    LinesRenderBatch(TetrahedralMesh& mesh);

    void initFromMesh(TetrahedralMesh& mesh);

    void render();
};

}

#endif // LINESRENDERBATCH_H
