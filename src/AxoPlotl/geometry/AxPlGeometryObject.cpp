#include "AxPlGeometryObject.h"
#include "imgui.h"

namespace AxoPlotl
{

int AxPlGeometryObject::id_counter_ = 0;

void AxPlGeometryObject::startRenderUI()
{
    ImGui::PushID(id_);

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
    if (ImGui::Selectable(("[" + type_name_ + "] " + name_).c_str())) {
        std::cout << name_ << " clicked!" << std::endl;
    }
    ImGui::SameLine();

    //---------------------
    // Rightclick Menu
    //---------------------
    if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
        ImGui::OpenPopup(("object_popup_" + std::to_string(id_)).c_str());
    }

    if (ImGui::BeginPopup(("object_popup_" + std::to_string(id_)).c_str())) {
        ImGui::Text("%s", std::to_string(id_).c_str());
        if (ImGui::MenuItem("Delete")) {
            // TODO
            deleted_ = true;
        }

        ImGui::EndPopup(); // Close the popup
    }
}

void AxPlGeometryObject::endRenderUI()
{
    // Control Visibility
    if (ImGui::Checkbox("##checkbox", &visible_))
    {
        // TODO: Visibility changed
        std::cout << "Set Visibility of " << name_ << ": " << visible_ << std::endl;
    }

    ImGui::PopID();
}

void TetrahedralMeshObject::updateRenderUI()
{
}

void ExplicitSurfaceObject::updateRenderUI()
{
    //-------------------
    // Text
    //-------------------
    ImGui::NewLine();
    if (ImGui::InputText("Input", input_buffer_, sizeof(input_buffer_)))
    {
        // Text Changed
    }

    //-------------------
    // Range Options
    //-------------------
    ImGui::InputFloat2("u Range", &f_.uMin);
    ImGui::InputFloat2("v Range", &f_.vMin);
}

void ImplicitSurfaceObject::updateRenderUI()
{

}

}
