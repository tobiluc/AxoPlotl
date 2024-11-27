#include "RenderBatchEdges.h"
#include "../utils/Globals.h"

namespace MV
{

template <typename MeshT>
void RenderBatchEdges<MeshT>::initFromMesh(MeshT& mesh)
{
    uint nVertices = 2*mesh.n_edges();
    uint nIndices = nVertices;

    vao.generateNew();
    vbo.generateNew(nVertices);
    ibo.generateNew(nIndices);

    std::vector<GLuint> indices_features;

    // create the vertex data and indices arrays
    auto prop = mesh.template request_edge_property<int>("AlgoHex::FeatureEdges");
    for (auto e_it = mesh.e_iter(); e_it.is_valid(); ++e_it)
    {
        auto eh = *e_it;
        int i = eh.idx();
        //bool boundary = mesh.is_boundary(eh);
        //int valence = mesh.valence(eh);
        auto heh = eh.halfedge_handle(0);
        auto vh0 = mesh.from_vertex_handle(heh);
        auto vh1 = mesh.to_vertex_handle(heh);
        Color col = COLORS[prop[eh]%COLORS.size()];

        auto d0 = toArray<float,6>(mesh.vertex(vh0), col);
        auto d1 = toArray<float,6>(mesh.vertex(vh1), col);

        setEdge(i, std::vector({d0, d1}));

        if (prop[eh])
        {
            indices_features.push_back(2*i+0);
            indices_features.push_back(2*i+1);
        }
    }
    ibo_features.generateNew(indices_features);
}

template <typename MeshT>
void RenderBatchEdges<MeshT>::render()
{
    Shader::EDGES_SHADER.use();

    // only rebuffer part of data that has changed
    vbo.bind();
    while (!updatedEdges.empty())
    {
        int i = updatedEdges.extract(updatedEdges.begin()).value(); // pop
        vbo.bufferSubData(2*i, 2);
    }

    vao.bind();
    vbo.enableAttributes();

    ibo.draw();

    vbo.disableAttributes();
    vao.unbind();
}

template class RenderBatchEdges<TetrahedralMesh>;
template class RenderBatchEdges<HexahedralMesh>;

}
