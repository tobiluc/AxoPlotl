#include "RenderBatchVertices.h"
#include "../utils/Utils.h"
#include "../utils/Globals.h"

namespace MV
{

template<typename MeshT>
void RenderBatchVertices<MeshT>::initFromMesh(MeshT& mesh)
{
    vao.generateNew();
    vbo.generateNew(mesh.n_vertices());
    ibo.generateNew(mesh.n_vertices());

    // create the vertex data
    auto prop = mesh.template request_vertex_property<int>("AlgoHex::FeatureVertices");
    for (auto v_it = mesh.v_iter(); v_it.is_valid(); ++v_it)
    {
        auto vh = *v_it;
        Color col = COLORS[prop[vh]%COLORS.size()];
        setVertex(vh.idx(), toArray<float,6>(mesh.vertex(vh), col));
    }

    vao.unbind();
}

template<typename MeshT>
void RenderBatchVertices<MeshT>::render(Shader& shader)
{
    vbo.bind();
    while (!updatedVertices.empty())
    {
        int i = updatedVertices.extract(updatedVertices.begin()).value(); // pop
        vbo.bufferSubData(i, 1);
    }

    vao.bind();
    vbo.enableAttributes();

    ibo.draw();

    vbo.disableAttributes();

    vao.unbind();
}

template class RenderBatchVertices<TetrahedralMesh>;
template class RenderBatchVertices<HexahedralMesh>;

}
