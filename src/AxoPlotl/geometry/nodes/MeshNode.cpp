#include "MeshNode.h"

namespace AxoPlotl
{

void MeshNode::addToRenderer(Scene* scene)
{
    renderer_.addMesh(mesh_, renderLoc_);
}

void MeshNode::renderUIBody(Scene* scene)
{
}

}
