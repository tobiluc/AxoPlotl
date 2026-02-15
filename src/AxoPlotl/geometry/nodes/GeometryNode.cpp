#include "GeometryNode.h"
#include "AxoPlotl/rendering/ImGuiRenderer.h"
#include <AxoPlotl/Scene.h>

namespace AxoPlotl
{

int GeometryNode::id_counter_ = 0;

void GeometryNode::renderUIHeader(Scene *scene)
{
    //---------------------
    // Draw colored circle
    //---------------------
    ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
    float circle_radius = 5.0f;
    ImVec2 circle_center = ImVec2(cursor_pos.x + circle_radius, cursor_pos.y + ImGui::GetTextLineHeight() * 0.5f);

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddCircleFilled(circle_center, circle_radius, IM_COL32(255*ui_color_[0], 255*ui_color_[1], 255*ui_color_[2], 255));

    ImGui::SetCursorScreenPos(ImVec2(cursor_pos.x + 2.0f * circle_radius + 4.0f, cursor_pos.y));

    bool deleted = false;

    //---------------------
    // Name
    //---------------------
    ImGui::Checkbox("", &visible());
    ImGui::SameLine();
    if (ImGui::Selectable(name_)) {
        // Open Popup with Settings
        ImGui::OpenPopup(("object_popup_" + std::to_string(id_)).c_str());
    }

    //---------------------
    // Options
    //---------------------
    ImGui::Checkbox("Expand", &show_ui_body_);

    //---------------------
    // Rightclick Menu
    //---------------------
    // if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
    //     ImGui::OpenPopup(("object_popup_" + std::to_string(id_)).c_str());
    // }

    if (ImGui::BeginPopup(("object_popup_" + std::to_string(id_)).c_str())) {

        // Name
        ImGui::SeparatorText("Info");
        ImGui::Text("id=%d", id_);
        ImGui::SameLine();
        ImGui::InputText("Name", name_, sizeof(name_));
        ImGui::Text("P/L/T = %zu/%zu/%zu",
            mesh_renderer_.n_vertex_points(),
            mesh_renderer_.n_edge_lines() + mesh_renderer_.n_cell_lines(),
            mesh_renderer_.n_face_triangles() + mesh_renderer_.n_cell_triangles()
        );

        ImGui::SeparatorText("Visualization Options");

        if (ImGui::Button("Zoom to Object")) {
            this->visible() = true;
            const auto& bbox = getBBox();
            scene->cameraSet().zoomToBox(bbox.first, bbox.second);
        }

        // Transform
        if (ImGui::BeginMenu("Transform")) {
            ImGui::InputFloat4x4("Matrix:", &transform_);
            if (ImGui::Button("Reset")) {transform_ = glm::mat4(1.0);} // Reset Transform
            ImGui::EndMenu();
        }

        // Material

        if (mesh_renderer_.n_vertex_points() > 0) {
            if (ImGui::BeginMenu("Vertices")) {
                ImGui::Checkbox("Show Vertices", &mesh_renderer_.render_vertices_);
                ImGui::SliderFloat("Size", &mesh_renderer_.point_size_, 1.f, 32.0f);
                ImGui::EndMenu();
            }
        }

        if (mesh_renderer_.n_edge_lines() > 0) {
            if (ImGui::BeginMenu("Edges")) {
                ImGui::Checkbox("Show Edges", &mesh_renderer_.render_edges_);
                ImGui::SliderFloat("Width", &mesh_renderer_.line_width_, 1.f, 16.0f);
                ImGui::EndMenu();
            }
        }

        if (mesh_renderer_.n_face_triangles() > 0) {
            if (ImGui::BeginMenu("Faces")) {
                ImGui::Checkbox("Show Faces", &mesh_renderer_.render_faces_);
                ImGui::Checkbox("Wireframe", &mesh_renderer_.faces_wireframe_);
                ImGui::EndMenu();
            }
        }

        if (mesh_renderer_.n_cell_triangles() > 0) {
            if (ImGui::BeginMenu("Cells")) {
                ImGui::Checkbox("Show Cells", &mesh_renderer_.render_cells_);
                ImGui::SliderFloat("Cell Scale", &mesh_renderer_.cell_scale_, 0.0f, 1.0f);
                ImGui::Checkbox("Wireframe", &mesh_renderer_.cells_wireframe_);
                ImGui::ColorEdit4("Outline Color", &mesh_renderer_.cells_outline_color_[0]);
                ImGui::EndMenu();
            }
        }

        if (ImGui::Checkbox("Enable Clip Box", &mesh_renderer_.clip_box_enabled_)) {
            // Initially, we set the clip box to the entire bounding box, so everthing is visible
            mesh_renderer_.clip_box_x_ = {bbox_.first[0], bbox_.second[0]};
            mesh_renderer_.clip_box_y_ = {bbox_.first[1], bbox_.second[1]};
            mesh_renderer_.clip_box_z_ = {bbox_.first[2], bbox_.second[2]};
        }
        if (mesh_renderer_.clip_box_enabled_) {
            ImGui::SliderFloat2("x", &mesh_renderer_.clip_box_x_[0], bbox_.first[0], bbox_.second[0]);
            ImGui::SliderFloat2("y", &mesh_renderer_.clip_box_y_[0], bbox_.first[1], bbox_.second[1]);
            ImGui::SliderFloat2("z", &mesh_renderer_.clip_box_z_[0], bbox_.first[2], bbox_.second[2]);
        }

        ImGui::SeparatorText("Edit");

        // Delete
        if (ImGui::MenuItem("Delete")) {
            deleted_ = true;
        }

        ImGui::EndPopup(); // Close the popup
    }
}

}
