#include "SphericalHarmonicNode.h"
#include "AxoPlotl/algorithms/parsing/reverse_polish.h"

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
        auto tokens = Parsing::tokenize(input_buffer_);

        Parsing::RPN rpn;
        Parsing::reversePolish(tokens, rpn);
        Parsing::Variables vars;
        Parsing::Functions funcs;
        Parsing::registerCommons(vars, funcs);

        std::function<float(Vec3f)> func = [&](const Vec3f& p) {
            vars["x"] = p.x;
            vars["y"] = p.y;
            vars["z"] = p.z;
            return Parsing::evaluate(rpn, vars, funcs);
        };

        // Update renderer
        this->f_.f = func;
        this->initRenderer(scene);
    }
}

void SphericalHarmonicNode::initRenderer(Scene* scene)
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
        .position = mesh.vertex(i), .color = Color(t,0,1-t), .normal = mesh.vertexNormal(i)});
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
