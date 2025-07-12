#include "ConvexPolygonNode.h"

namespace AxoPlotl
{

void ConvexPolygonNode::initRenderer(Scene* scene)
{
    //bbox_.compute({vertices_[0],vertices_[1],vertices_[2]});


    Vec3f normal = glm::normalize( glm::cross(vertices_[2]-vertices_[0], vertices_[1]-vertices_[0]));

    GL::MeshRenderer::Data data;
    for (uint i = 0; i < vertices_.size(); ++i) {
        data.pointAttribs.push_back(GL::MeshRenderer::VertexPointAttrib{.position = vertices_[i], .color = Color::WHITE});
        data.lineAttribs.push_back(GL::MeshRenderer::VertexLineAttrib{.position = vertices_[i], .color = Color::WHITE});
        data.triangleAttribs.push_back(GL::MeshRenderer::VertexTriangleAttrib{.position = vertices_[i], .color = Color::WHITE, .normal = normal});

        data.pointIndices.push_back(i);
        data.lineIndices.push_back(i);
        data.lineIndices.push_back((i+1)%vertices_.size());
    }
    for (uint i = 1; i < vertices_.size()-1; ++i) {
        data.triangleIndices.push_back(0);
        data.triangleIndices.push_back(i);
        data.triangleIndices.push_back(i+1);
    }
    mesh_renderer_.updateData(data);
}

void ConvexPolygonNode::renderUIBody(Scene* scene)
{
    //-------------------
    // Text
    //-------------------
    for (int i = 0; i < vertices_.size(); ++i) {
        ImGui::PushID(i);
        char lbl[3];
        ImGui::Text("%d", i);
        ImGui::SameLine();
        if (ImGui::InputFloat3("##xx", &vertices_[i].x)) {}
        ImGui::PopID();
    }

    // Add/Remove Vertices
    ImGui::SameLine();
    if (vertices_.size()>3 && ImGui::Button("-")) {vertices_.erase(vertices_.end()-1);}
    ImGui::SameLine();
    if (ImGui::Button("+")) {vertices_.push_back(Vec3f(0,0,0));}

    // Confirm
    if (ImGui::Button("Confirm")) {

        // Update Renderer
        this->initRenderer(scene);
    }
}

}
