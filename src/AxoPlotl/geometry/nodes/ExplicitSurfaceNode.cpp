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
        this->initRenderer(scene);
    }
    // ImGui::SameLine();
    // ImGui::Checkbox("Continous Evaluation", &continous_evaluation_);
}

void ExplicitSurfaceNode::initRenderer(Scene* scene)
{
    PolygonMesh mesh;
    createQuads(f_, mesh, resolution_);
    GL::MeshRenderer::Data data;

    for (uint i = 0; i < mesh.vertices().size(); ++i) {
        Vec3f p = mesh.vertex(i);
        float t = i/((float)mesh.vertices().size()-1);

        data.lineAttribs.push_back(GL::MeshRenderer::VertexLineAttrib{
            .position = p,
            .color = Color(t,0,1-t)
        });

        data.triangleAttribs.push_back(GL::MeshRenderer::VertexTriangleAttrib{
        .position = p,
        .color = getColorOnSphere(p.x,p.y,p.z),
        .normal = mesh.vertexNormal(i)});

        for (uint j : mesh.neighbors(i)) {
            if (j > i) {
                data.lineIndices.push_back(i);
                data.lineIndices.push_back(j);
            }
        }

    }
    for (uint i = 0; i < mesh.polygons().size(); ++i) {
        for (uint j = 1; j < mesh.polygon(i).size()-1; ++j) {
            data.triangleIndices.push_back(mesh.polygon(i)[0]);
            data.triangleIndices.push_back(mesh.polygon(i)[j]);
            data.triangleIndices.push_back(mesh.polygon(i)[j+1]);
        }
    }
    mesh_renderer_.updateData(data);
}

std::pair<Vec3f,Vec3f> ExplicitSurfaceNode::getBBox()
{
    Vec3f min(std::numeric_limits<float>::infinity());
    Vec3f max(-std::numeric_limits<float>::infinity());
    for (uint i = 0; i <= resolution_; ++i) {
        float s, t;
        for (int i = 0; i <= resolution_; ++i) {
            s = f_.uMin + i * (f_.uMax - f_.uMin) / resolution_;
            for (int j = 0; j <= resolution_; ++j) {
                t = f_.vMin + j * (f_.vMax - f_.vMin) / resolution_;
                Vec3f f = f_.f(s,t);
                for (int a = 0; a < 3; ++a) {
                    min[a] = std::min(min[a], f[a]);
                    max[a] = std::max(max[a], f[a]);
                }
            }
        }
    }
    return {min, max};
}

}
