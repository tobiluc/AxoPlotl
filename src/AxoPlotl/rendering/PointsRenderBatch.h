#ifndef POINTSRENDERBATCH_H
#define POINTSRENDERBATCH_H

#include <sys/types.h>
#include "../utils/Typedefs.h"
#include "RenderBatch.h"

namespace AxPl
{

class PointsRenderBatch : public RenderBatch<Point>
{
public:
    PointsRenderBatch(size_t max_num_vertices);

    PointsRenderBatch(TetrahedralMesh& mesh);

    void initFromMesh(TetrahedralMesh& mesh);

    void render();
};

}

#endif // POINTSRENDERBATCH_H
