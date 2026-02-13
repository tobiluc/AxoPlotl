#include "ImplicitSurfaceNode.h"
#include <ibex/ibex.hpp>

namespace AxoPlotl
{

void ImplicitSurfaceNode::init(Scene *scene)
{
    SurfaceMesh mesh;
    createTrianglesAMC(f_, mesh, initial_resolution_, octree_depth_);
    // VolumeMesh mesh = volume_mesh(triangles);
    // GL::MeshRenderer::Data data;
    // GL::MeshRenderer::createData(mesh, data);
    // mesh_renderer_.updateData(data);
    GL::MeshRenderer::Data data;

    for (int i = 0; i < mesh.n_vertices(); ++i) {
        auto p = mesh.point(i);
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

    this->bbox_ = {Vec3f(f_.xMin,f_.yMin,f_.zMin),Vec3f(f_.xMax,f_.yMax,f_.zMax)};
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
