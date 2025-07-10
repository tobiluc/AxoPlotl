#include "MeshNode.h"

namespace AxoPlotl
{

void MeshNode::addToRenderer(Scene* scene)
{
    //renderer_.addMesh(mesh_, renderLoc_);
    mesh_renderer_.initFromMesh(mesh_);
}

void MeshNode::renderUIBody(Scene* scene)
{
}

}
