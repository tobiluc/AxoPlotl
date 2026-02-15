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

    bbox_ = {Vec3f(std::numeric_limits<float>::infinity()), Vec3f(-std::numeric_limits<float>::infinity())};

    VolumeMeshRenderer::StaticData data;
    for (int i = 0; i < mesh.n_vertices(); ++i) {

        // Push Vertex Position
        auto p = mesh.point(i);
        data.positions_.push_back(Vec4f(p[0],p[1],p[2],1));

        // BBox
        for (int a = 0; a < 3; ++a) {
            bbox_.first[a] = std::min(bbox_.first[a], p[a]);
            bbox_.second[a] = std::max(bbox_.second[a], p[a]);
        }
    }

    // Lines
    std::vector<VolumeMeshRenderer::EdgeData> e_data;
    data.n_edges_ = mesh.n_edges();
    int e_idx(0);
    for (const auto& e : mesh.edges()) {
        data.edge_draw_vertices_.push_back({
        .vertex_index=static_cast<uint32_t>(e.vertex(0)),
        .edge_index=static_cast<uint32_t>(e_idx)
        });
        data.edge_draw_vertices_.push_back({
        .vertex_index=static_cast<uint32_t>(e.vertex(1)),
        .edge_index=static_cast<uint32_t>(e_idx)
        });

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
}

}
