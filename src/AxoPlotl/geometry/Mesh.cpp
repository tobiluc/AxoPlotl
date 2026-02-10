#include "Mesh.hpp"
#include <AxoPlotl/utils/Utils.h>

namespace AxoPlotl
{

void SurfaceMeshNode::init(Scene* _scene)
{
    // Compute the Bounding Box
    bbox_.first = Vec3f(std::numeric_limits<float>::infinity());
    bbox_.second = Vec3f(-std::numeric_limits<float>::infinity());
    for (int i = 0; i < mesh_.n_vertices(); ++i) {
        auto p = mesh_.point(OpenMesh::VertexHandle(i));
        for (int j = 0; j < 3; ++j) {
            bbox_.first[j] = std::min(bbox_.first[j], p[j]);
            bbox_.second[j] = std::max(bbox_.second[j], p[j]);
        }
    }

    // Create the Render Data
    GL::MeshRenderer::Data data;

    // Vertices
    for (const auto& v : mesh_.vertices()) {
        data.pointAttribs.push_back(GL::MeshRenderer::VertexPointAttrib{
            .position = toVec3<Vec3f>(mesh_.point(v)),
            .color = Vec4f(0.4,0.4,0.4,1)
        });
        data.pointIndices.push_back(v.idx());
    }

    // Edges
    for (const auto& e : mesh_.edges()) {
        data.lineAttribs.push_back(GL::MeshRenderer::VertexLineAttrib{
            .position = toVec3<Vec3f>(mesh_.point(e.v0())),
            .color = Vec4f(0,0,0,1)
        });
        data.lineAttribs.push_back(GL::MeshRenderer::VertexLineAttrib{
            .position = toVec3<Vec3f>(mesh_.point(e.v1())),
            .color = Vec4f(0,0,0,1)
        });
        data.lineIndices.push_back(data.lineAttribs.size()-2);
        data.lineIndices.push_back(data.lineAttribs.size()-1);
    }

    // Faces
    for (const auto& f : mesh_.faces()) {
        const auto& vhs = f.vertices();

        auto n = mesh_.normal(f);
        size_t n_verts(0);
        int idx(data.triangleAttribs.size());
        for (const auto& vh : vhs) {
            data.triangleAttribs.push_back(GL::MeshRenderer::VertexTriangleAttrib{
                .position = toVec3<Vec3f>(mesh_.point(vh)),
                .color = Vec4f(0.4,0.4,0.4,1),
                .normal = Vec3f(n[0],n[1],n[2]),
                .buffer = 0.0f
            });
            ++n_verts;
        }

        for (uint j = 1; j < n_verts; ++j) {
            data.triangleIndices.push_back(idx);
            data.triangleIndices.push_back(idx+j);
            data.triangleIndices.push_back(idx+j+1);
        }
    }

    renderer_.updateData(data);
}

void SurfaceMeshNode::renderUIBody(Scene* scene)
{

}

void VolumeMeshNode::init(Scene* _scene)
{
    // Compute the Bounding Box
    bbox_.first = Vec3f(std::numeric_limits<float>::infinity());
    bbox_.second = Vec3f(-std::numeric_limits<float>::infinity());
    for (int i = 0; i < mesh_.n_vertices(); ++i) {
        auto p = mesh_.vertex(OpenVolumeMesh::VertexHandle(i));
        for (int j = 0; j < 3; ++j) {
            bbox_.first[j] = std::min(bbox_.first[j], p[j]);
            bbox_.second[j] = std::max(bbox_.second[j], p[j]);
        }
    }

    // Create the Render Data
    GL::MeshRenderer::Data data;
}

void VolumeMeshNode::renderUIBody(Scene* scene)
{

}

}
