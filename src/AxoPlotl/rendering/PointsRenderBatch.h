#ifndef POINTSRENDERBATCH_H
#define POINTSRENDERBATCH_H

#include <sys/types.h>
#include "../utils/Typedefs.h"
#include "RenderBatch.h"

namespace AxoPlotl
{

class PointsRenderBatch : public RenderBatch<Point>
{
private:
    float pointSize_ = 5.0f; // Size of a point in pixels

public:
    PointsRenderBatch(size_t max_num_vertices);

    PointsRenderBatch(TetrahedralMesh& mesh);

    void initFromMesh(TetrahedralMesh& mesh);

    void render();

    inline float& pointSize() {
        return pointSize_;
    }

};

}

#endif // POINTSRENDERBATCH_H
