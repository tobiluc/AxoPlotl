#include "ExplicitSurfaceNode.h"
#include <ibex/ibex.hpp>

namespace AxoPlotl
{

void ExplicitSurfaceNode::renderUIBody(Scene* scene)
{
    //-------------------
    // Text
    //-------------------
    ImGui::NewLine();
    ImGui::Text("x(u,v) = ");
    ImGui::SameLine();
    bool input_changed = false;
    if (ImGui::InputText("##x", input_buffer_x_, sizeof(input_buffer_x_))) {input_changed = true;} // x changed
    ImGui::Text("y(u,v) = ");
    ImGui::SameLine();
    if (ImGui::InputText("##y", input_buffer_y_, sizeof(input_buffer_y_))) {input_changed = true;}// y changed
    ImGui::Text("z(u,v) = ");
    ImGui::SameLine();
    if (ImGui::InputText("##z", input_buffer_z_, sizeof(input_buffer_z_))) {input_changed = true;}// z changed

    //-------------------
    // Range Options
    //-------------------
    if (ImGui::InputFloat2("u Range", &f_.uMin)) {input_changed = true;};
    if (ImGui::InputFloat2("v Range", &f_.vMin)) {input_changed = true;};
    if (ImGui::SliderInt("Resolution", &resolution_, 2, 128)) {input_changed = true;};

    //-------------------
    // Update
    //-------------------
    if (ImGui::Button("Confirm") || (continous_evaluation_ && input_changed)) {
        // Parse Input Text
        ibex::Functions funcs = ibex::common_functions();
        ibex::Variables vars = ibex::common_variables();
        auto rpn_x = ibex::generate_postfix(ibex::tokenize(input_buffer_x_));
        auto rpn_y = ibex::generate_postfix(ibex::tokenize(input_buffer_y_));
        auto rpn_z = ibex::generate_postfix(ibex::tokenize(input_buffer_z_));

        std::function<Vec3f(float,float)> func = [&](float u, float v) {
            vars["u"] = u;
            vars["v"] = v;
            return Vec3f(
                ibex::eval_postfix(rpn_x, vars, funcs),
                ibex::eval_postfix(rpn_y, vars, funcs),
                ibex::eval_postfix(rpn_z, vars, funcs)
            );
        };

        // Update renderer
        this->f_.f = func;
        this->init(scene);
    }
    // ImGui::SameLine();
    // ImGui::Checkbox("Continous Evaluation", &continous_evaluation_);
}

void ExplicitSurfaceNode::init(Scene* scene)
{
    SurfaceMesh mesh;
    createQuads(f_, mesh, resolution_);

    bbox_ = {Vec3f(std::numeric_limits<float>::infinity()), Vec3f(-std::numeric_limits<float>::infinity())};

    GL::MeshRenderer::Data data;

    for (int i = 0; i < mesh.n_vertices(); ++i) {
        auto p = mesh.point(i);

        for (int a = 0; a < 3; ++a) {
            bbox_.first[a] = std::min(bbox_.first[a], p[a]);
            bbox_.second[a] = std::max(bbox_.second[a], p[a]);
        }

        data.lineAttribs.push_back(GL::MeshRenderer::VertexLineAttrib{
            .position = p,
            .color = Vec4f(0,0,0,1)
        });

        data.triangleAttribs.push_back(GL::MeshRenderer::VertexTriangleAttrib{
            .position = p,
            .color = getColorOnSphere(p[0],p[1],p[2]),
            .normal = Vec3f(1,0,0)});

        for (auto vv_it = mesh.vv_iter(i); vv_it.is_valid(); ++vv_it) {
            int j = *vv_it;
            if (j > i) {
                data.lineIndices.push_back(i);
                data.lineIndices.push_back(j);
            }
        }
    }
    for (const auto& f : mesh.faces()) {
        for (uint j = 1; j < f.valence()-1; ++j) {
            data.face_triangle_indices_.push_back(f.vertices()[0]);
            data.face_triangle_indices_.push_back(f.vertices()[j]);
            data.face_triangle_indices_.push_back(f.vertices()[j+1]);
        }
    }
    mesh_renderer_.updateData(data);

}

}
