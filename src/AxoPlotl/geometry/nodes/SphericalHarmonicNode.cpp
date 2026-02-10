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
    PolygonMesh mesh;
    createQuads(f_, mesh, resolution_);
    GL::MeshRenderer::Data data;

    float min = std::numeric_limits<float>::infinity(), max = -std::numeric_limits<float>::infinity();
    for (uint i = 0; i < mesh.vertices().size(); ++i) {
        float val = f_(mesh.vertex(i));
        if (val < min) {min = val;}
        if (val > max) {max = val;}
    }

    for (uint i = 0; i < mesh.vertices().size(); ++i) {
        float val = f_(mesh.vertex(i));
        float t = (val - min) / (max - min);
        data.triangleAttribs.push_back(GL::MeshRenderer::VertexTriangleAttrib{
        .position = mesh.vertex(i), .color = Vec4f(t,0,1-t,1), .normal = mesh.vertexNormal(i)});
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

}
