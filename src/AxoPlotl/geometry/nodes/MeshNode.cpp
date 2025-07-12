#include "MeshNode.h"

namespace AxoPlotl
{

void MeshNode::initRenderer(Scene* scene)
{
    //renderer_.addMesh(mesh_, renderLoc_);
    GL::MeshRenderer::Data data;
    GL::MeshRenderer::createData(mesh_, data);
    mesh_renderer_.updateData(data);
}

void MeshNode::renderUIBody(Scene* scene)
{
}

}
