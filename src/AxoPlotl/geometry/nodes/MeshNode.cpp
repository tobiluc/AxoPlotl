#include "MeshNode.h"

namespace AxoPlotl
{

void MeshNode::initRenderer(Scene* scene)
{
    //renderer_.addMesh(mesh_, renderLoc_);
    mesh_renderer_.init(mesh_);
}

void MeshNode::renderUIBody(Scene* scene)
{
}

}
