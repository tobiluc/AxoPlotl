#include "GeometryObject.h"
#include "AxoPlotl/parsing/Scope.h"
#include "AxoPlotl/parsing/parsing.h"
#include "AxoPlotl/parsing/tokens.h"
#include "AxoPlotl/rendering/ImGuiRenderer.h"
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

        ImGui::Text("%s (%d)", name_.c_str(), id_);
        ImGui::Separator();

        // Transform
        ImGui::InputFloat4x4("Transform Matrix:", &transform_);
        if (ImGui::Button("Reset Transform")) {transform_ = glm::mat4(1.0);} // Reset Transform
        ImGui::Separator();

        // // Mesh Visibility
        // ImGui::Checkbox("Show Cells", &settings.showCells);
        // ImGui::Checkbox("Show Faces", &settings.showFaces);
        // ImGui::Checkbox("Show Edges", &settings.showEdges);
        // ImGui::Checkbox("Show Vertices", &settings.showVertices);

        // Material
        auto& settings = renderer_.settings;
        ImGui::SliderFloat("Vertex Size", &settings.pointSize, 1.f, 16.0f);
        ImGui::SliderFloat("Edge Width", &settings.lineWidth, 1.f, 16.0f);
        ImGui::SliderFloat("Cell Scale", &settings.cellScale, 0.0f, 1.0f);
        ImGui::SliderFloat("Polygon Outline Width", &settings.outlineWidth, 0.0f, 12.0f);
        ImGui::ColorEdit3("Polygon Outline Color", &settings.outlineColor[0]);
        ImGui::Checkbox("Use Override Color", &settings.useColorOverride);
        ImGui::ColorEdit3("Override Color", &settings.colorOverride[0]);
        ImGui::ColorEdit3("Ambient", &settings.light.ambient[0]);
        ImGui::ColorEdit3("Diffuse", &settings.light.diffuse[0]);
        ImGui::ColorEdit3("Specular", &settings.light.specular[0]);
        ImGui::Separator();

        // Delete
        if (ImGui::MenuItem("Delete")) {
            deleted_ = true;
            renderer_.remove(renderLoc_);
        }

        ImGui::EndPopup(); // Close the popup
    }
}

void AxPlGeometryObject::endRenderUI()
{
    //---------------------
    // Control Visibility
    //---------------------
    if (ImGui::Checkbox("##checkbox", &renderer_.settings.visible))
    {
        // TODO: Visibility changed
        //std::cout << "Set Visibility of " << name_ << ": " << visible_ << std::endl;
    }

    ImGui::PopID();
}

void TetrahedralMeshObject::addToRenderer()
{
    renderer_.addTetMesh(mesh_, renderLoc_);
}

void TetrahedralMeshObject::updateRenderUI()
{
}

void HexahedralMeshObject::addToRenderer()
{
    renderer_.addHexMesh(mesh_, renderLoc_);
}

void HexahedralMeshObject::updateRenderUI()
{
}

void ExplicitSurfaceObject::addToRenderer()
{
    TriangleMesh mesh;
    createTriangles(f_, mesh, resolution_);
    std::vector<Rendering::Triangle> tris;
    for (uint i = 0; i < mesh.triangles.size(); ++i) {
        tris.emplace_back(
            mesh.vertices[mesh.triangles[i][0]],
            mesh.vertices[mesh.triangles[i][1]],
            mesh.vertices[mesh.triangles[i][2]],
            ui_color_ // TODO: Display correct color
        );
    }
    renderer_.addTriangles(tris, renderLoc_);
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
    ImGui::SliderFloat2("u Range", &f_.uMin, -10.0f, 10.0f);
    ImGui::SliderFloat2("v Range", &f_.vMin, -10.0f, 10.0f);
}

void ImplicitSurfaceObject::addToRenderer()
{
    TriangleMesh mesh;
    createTriangles(f_, mesh, resolution_);
    std::vector<Rendering::Triangle> tris;
    for (uint i = 0; i < mesh.triangles.size(); ++i) {
        tris.emplace_back(
            mesh.vertices[mesh.triangles[i][0]],
            mesh.vertices[mesh.triangles[i][1]],
            mesh.vertices[mesh.triangles[i][2]],
            ui_color_ // TODO: Display correct color
            );
    }
    renderer_.addTriangles(tris, renderLoc_);
}

void ImplicitSurfaceObject::updateRenderUI()
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
    ImGui::SliderInt("Resolution", &resolution_, 2, 64);

    //-------------------
    // Update
    //-------------------
    if (ImGui::Button("Confirm")) {
        // Parse Input Text
        auto tokens = Parsing::tokenize(input_buffer_);
        for (auto& token : tokens) std::cout << token << std::endl;

        // Tranform AST into a function of x, y, z
        auto root = Parsing::Parser(tokens).parse();
        root->print();
        std::cout << std::endl;
        std::function<float(Vec3f)> func = [&root](const Vec3f& v) {
            Scope scope;
            scope.setVariable("x", std::make_shared<Parsing::ScalarNode>(v.x));
            scope.setVariable("y", std::make_shared<Parsing::ScalarNode>(v.y));
            scope.setVariable("z", std::make_shared<Parsing::ScalarNode>(v.z));
            auto value = root->evaluate(scope);
            if (!value) {std::cerr << "Value is NULL" << std::endl;}
            auto scalar = dynamic_cast<Parsing::ScalarValue*>(value.get());
            if (scalar) {return (float)(scalar->value);}
            std::cerr << "Value is not a Scalar" << std::endl;
            return 0.0f;
        };

        // Update renderer
        this->f_.f = func;
        renderer_.remove(renderLoc_);
        this->addToRenderer();

        //std::cout << "f(1,2,3) = " << func(1, 2, 3) << std::endl;
    }
}

}
