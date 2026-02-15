#include "ImplicitSurfaceNode.h"
#include <ibex/ibex.hpp>

namespace AxoPlotl
{

void ImplicitSurfaceNode::init(Scene *scene)
{
    SurfaceMesh mesh;
    createTrianglesAMC(f_, mesh, initial_resolution_, octree_depth_);

    VolumeMeshRenderer::StaticData data;
    for (int i = 0; i < mesh.n_vertices(); ++i) {

        // Push Vertex Position
        auto p = mesh.point(i);
        data.positions_.push_back(Vec4f(p[0],p[1],p[2],1));
    }

    // Lines
    std::vector<VolumeMeshRenderer::EdgeData> e_data;
    data.n_edges_ = mesh.n_edges();
    int e_idx(0);
    for (const auto& e : mesh.edges()) {
        data.edge_draw_vertices_.push_back({
                                            .vertex_index=static_cast<uint32_t>(e.vertex(0)),
                                            .edge_index=static_cast<uint32_t>(e_idx)});
        data.edge_draw_vertices_.push_back({
                                            .vertex_index=static_cast<uint32_t>(e.vertex(1)),
                                            .edge_index=static_cast<uint32_t>(e_idx)});

        const auto& p = color_on_sphere(mesh.point(e.vertex(0)));
        e_data.push_back({.property = p});
        e_idx++;
    }

    // Faces
    std::vector<VolumeMeshRenderer::FaceData> f_data;
    vol_rend_.n_faces_ = mesh.n_faces();
    int f_idx(0);
    for (const auto& f : mesh.faces()) {
        for (int i = 1; i < f.valence()-1; ++i) {
            data.face_triangle_draw_vertices_.push_back({
                                                         .vertex_index=static_cast<uint32_t>(f.vertices()[0]),
                                                         .face_index=static_cast<uint32_t>(f_idx)});
            data.face_triangle_draw_vertices_.push_back({
                                                         .vertex_index=static_cast<uint32_t>(f.vertices()[i]),
                                                         .face_index=static_cast<uint32_t>(f_idx)});
            data.face_triangle_draw_vertices_.push_back({
                                                         .vertex_index=static_cast<uint32_t>(f.vertices()[i+1]),
                                                         .face_index=static_cast<uint32_t>(f_idx)});
        }

        const auto& p = color_on_sphere(mesh.point(f.vertices()[0]));
        f_data.push_back({.property = p});
        f_idx++;
    }

    vol_rend_.init(data);
    vol_rend_.update_edge_data(e_data);
    vol_rend_.update_face_data(f_data);

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
