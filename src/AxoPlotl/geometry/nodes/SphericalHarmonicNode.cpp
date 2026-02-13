#include "SphericalHarmonicNode.h"
#include <ibex/ibex.hpp>

namespace AxoPlotl
{

void SphericalHarmonicNode::renderUIBody(Scene* scene)
{
    //-------------------
    // Text
    //-------------------
    ImGui::NewLine();
    ImGui::Text("f(x,y,z) = ");
    ImGui::SameLine();
    if (ImGui::InputText("##x", input_buffer_, sizeof(input_buffer_))) {} // x changed

    //-------------------
    // Range Options
    //-------------------
    if (ImGui::SliderInt("Resolution", &resolution_, 2, 128)) {};

    //-------------------
    // Update
    //-------------------
    if (ImGui::Button("Confirm")) {
        // Parse Input Text
        ibex::Variables vars = ibex::common_variables();
        ibex::Functions funcs = ibex::common_functions();
        auto rpn = ibex::generate_postfix(ibex::tokenize(input_buffer_));

        std::function<float(Vec3f)> func = [&](const Vec3f& p) {
            vars["x"] = p.x;
            vars["y"] = p.y;
            vars["z"] = p.z;
            return ibex::eval_postfix(rpn, vars, funcs);
        };

        // Update renderer
        this->f_.f = func;
        this->init(scene);
    }
}

void SphericalHarmonicNode::init(Scene* scene)
{
    SurfaceMesh mesh;
    createQuads(f_, mesh, resolution_);
    GL::MeshRenderer::Data data;

    float min = std::numeric_limits<float>::infinity(), max = -std::numeric_limits<float>::infinity();
    for (uint i = 0; i < mesh.n_vertices(); ++i) {
        auto p = mesh.point(i);
        float val = f_(p);
        if (val < min) {min = val;}
        if (val > max) {max = val;}

        float t = (val - min) / (max - min);
        data.triangleAttribs.push_back(GL::MeshRenderer::VertexTriangleAttrib{
            .position = p,
            .color = Vec4f(t,0,1-t,1),
            .normal = Vec3f(1,0,0)});
    }

    for (const auto& f : mesh.faces()) {
        std::vector<int> is;
        for (const auto& v : f.vertices()) {is.push_back(v);}
        for (uint j = 1; j < is.size()-1; ++j) {
            data.face_triangle_indices_.push_back(is[0]);
            data.face_triangle_indices_.push_back(is[j]);
            data.face_triangle_indices_.push_back(is[j+1]);
        }
    }
    mesh_renderer_.updateData(data);

    this->bbox_ = {Vec3f(-1,-1,-1),Vec3f(1,1,1)};
}

}
