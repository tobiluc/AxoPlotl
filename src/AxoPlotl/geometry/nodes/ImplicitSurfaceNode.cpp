#include "ImplicitSurfaceNode.h"
#include <ibex/ibex.hpp>

namespace AxoPlotl
{

void ImplicitSurfaceNode::init(Scene *scene)
{
    PolyhedralMesh mesh;
    TriangleMesh triangles;
    createTrianglesAMC(f_, triangles, initial_resolution_, octree_depth_);

    for (const auto& p : triangles.vertices) {
        mesh.add_vertex(toVec3<OVM::Vec3d>(p));
    }

    for (const auto& t : triangles.triangles) {
        mesh.add_face({OVM::VH(t[0]), OVM::VH(t[1]), OVM::VH(t[2])});
    }

    GL::MeshRenderer::Data data;
    GL::MeshRenderer::createData(mesh, data);
    mesh_renderer_.updateData(data);
}

void ImplicitSurfaceNode::renderUIBody(Scene *scene)
{
    //-------------------
    // Text
    //-------------------
    ImGui::NewLine();
    ImGui::Text("f(x,y,z) = ");
    ImGui::SameLine();
    if (ImGui::InputText(" = 0", input_buffer_, sizeof(input_buffer_)))
    {
        // Text Changed
    }

    //-------------------
    // Range Options
    //-------------------
    ImGui::InputFloat2("x Range", &f_.xMin);
    ImGui::InputFloat2("y Range", &f_.yMin);
    ImGui::InputFloat2("z Range", &f_.zMin);
    ImGui::SliderInt("Init. Resolution", &initial_resolution_, 8, 32);
    ImGui::SliderInt("Octree Depth", &octree_depth_, 0, 3);

    //-------------------
    // Update
    //-------------------
    if (ImGui::Button("Confirm")) {
        // Parse Input Text
        ibex::Functions funcs = ibex::common_functions();
        ibex::Variables vars = ibex::common_variables();
        auto rpn = ibex::generate_postfix(ibex::tokenize(input_buffer_));

        auto func = [&](const Vec3f& v) {
            vars["x"] = v.x;
            vars["y"] = v.y;
            vars["z"] = v.z;
            return ibex::eval_postfix(rpn, vars, funcs);
        };

        // Update renderer
        this->f_.f = func;
        this->init(scene);
    }
}

}
