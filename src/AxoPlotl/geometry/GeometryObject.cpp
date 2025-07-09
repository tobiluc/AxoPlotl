#include "GeometryObject.h"
#include "AxoPlotl/parsing/Scope.h"
#include "AxoPlotl/parsing/parsing.h"
#include "AxoPlotl/parsing/tokens.h"
#include "AxoPlotl/rendering/ImGuiRenderer.h"
#include "imgui.h"
#include "AxoPlotl/Scene.h"

namespace AxoPlotl
{

int AxPlGeometryObject::id_counter_ = 0;

void AxPlGeometryObject::renderUIHeader(Scene *scene)
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
    if (ImGui::Selectable(("[" + type_name_ + "] " + name_).c_str())) {
        // Open Popup with Settings
        ImGui::OpenPopup(("object_popup_" + std::to_string(id_)).c_str());
    }

    //---------------------
    // Options
    //---------------------
    // ImGui::SameLine();
    // if (ImGui::Button("Zoom to")) {
    //     scene->zoomToObject(id());
    // }
    ImGui::SameLine();
    ImGui::Checkbox("Expand", &show_ui_body_);
    ImGui::SameLine();
    ImGui::Checkbox("Visible", &renderer_.settings.visible);

    //---------------------
    // Rightclick Menu
    //---------------------
    // if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
    //     ImGui::OpenPopup(("object_popup_" + std::to_string(id_)).c_str());
    // }

    if (ImGui::BeginPopup(("object_popup_" + std::to_string(id_)).c_str())) {

        // Name
        ImGui::Text("Settings (Object Id: %d)", id_);
        ImGui::InputText("Name", name_, sizeof(name_));
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
        ImGui::Checkbox("Wireframe", &settings.wireframe);
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
            renderer_.clear(renderLoc_);
        }

        ImGui::EndPopup(); // Close the popup
    }
}

void TetrahedralMeshObject::addToRenderer(Scene* scene)
{
    bbox_ = computeBoundingBox(mesh_);
    renderer_.addTetMesh(mesh_, renderLoc_);
}

void TetrahedralMeshObject::renderUIBody(Scene* scene)
{
}

void HexahedralMeshObject::addToRenderer(Scene* scene)
{
    bbox_ = computeBoundingBox(mesh_);
    renderer_.addHexMesh(mesh_, renderLoc_);
}

void HexahedralMeshObject::renderUIBody(Scene* scene)
{
}

void MeshObject::addToRenderer(Scene* scene)
{
    bbox_.compute(mesh_.vertices());
    renderer_.addMesh(mesh_, renderLoc_);
}

void MeshObject::renderUIBody(Scene* scene)
{
}

void ExplicitSurfaceObject::addToRenderer(Scene* scene)
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
    bbox_.compute(mesh.vertices);
    renderer_.addTriangles(tris, renderLoc_);
}

void ExplicitSurfaceObject::renderUIBody(Scene* scene)
{
    //-------------------
    // Text
    //-------------------
    ImGui::NewLine();
    ImGui::Text("x(u,v) = ");
    ImGui::SameLine();
    if (ImGui::InputText("##x", input_buffer_x_, sizeof(input_buffer_x_))) {} // x changed
    ImGui::Text("y(u,v) = ");
    ImGui::SameLine();
    if (ImGui::InputText("##y", input_buffer_y_, sizeof(input_buffer_y_))) {}// y changed
    ImGui::Text("z(u,v) = ");
    ImGui::SameLine();
    if (ImGui::InputText("##z", input_buffer_z_, sizeof(input_buffer_z_))) {}// z changed

    //-------------------
    // Range Options
    //-------------------
    ImGui::InputFloat2("u Range", &f_.uMin);
    ImGui::InputFloat2("v Range", &f_.vMin);
    ImGui::SliderInt("Resolution", &resolution_, 2, 64);

    //-------------------
    // Update
    //-------------------
    if (ImGui::Button("Confirm")) {
        // Parse Input Text
        auto tokens_x = Parsing::tokenize(input_buffer_x_);
        auto tokens_y = Parsing::tokenize(input_buffer_y_);
        auto tokens_z = Parsing::tokenize(input_buffer_z_);

        // Tranform ASTa into functiona of u, v
        auto root_x = Parsing::Parser(tokens_x).parse();
        auto root_y = Parsing::Parser(tokens_y).parse();
        auto root_z = Parsing::Parser(tokens_z).parse();

        std::function<Vec3f(float,float)> func = [&](float u, float v) {
            Scope scope;
            scope.setVariable("u", std::make_shared<Parsing::ScalarNode>(u));
            scope.setVariable("v", std::make_shared<Parsing::ScalarNode>(v));

            auto value_x = root_x->evaluate(scope);
            auto value_y = root_y->evaluate(scope);
            auto value_z = root_z->evaluate(scope);

            return Vec3f(
                std::visit([](auto&& v) {
                    using T = std::decay_t<decltype(v)>;
                    if constexpr (std::is_same_v<T, double>) {return v;}
                    else {return 0.0;}
                }, value_x.val),
                std::visit([](auto&& v) {
                    using T = std::decay_t<decltype(v)>;
                    if constexpr (std::is_same_v<T, double>) {return v;}
                    else {return 0.0;}
                }, value_y.val),
                std::visit([](auto&& v) {
                    using T = std::decay_t<decltype(v)>;
                    if constexpr (std::is_same_v<T, double>) {return v;}
                    else {return 0.0;}
                }, value_z.val)
            );
        };

        // Update renderer
        this->f_.f = func;
        renderer_.clear(renderLoc_);
        this->addToRenderer(scene);
    }
}

void ImplicitSurfaceObject::addToRenderer(Scene *scene)
{
    TriangleMesh mesh;
    Octree tree;
    createTrianglesAMC(f_, mesh, tree, resolution_);

    // Octree
    // HexahedralMesh treemesh;
    // for (u32 i = 0; i < tree.numNodes(); ++i) {
    //     if (!tree.isActive(i)) {continue;}
    //     auto c = tree.getNodeBounds(i).corners<OVM::Vec3d>();
    //     std::vector<OVM::VH> vhs;
    //     for (u32 j = 0; j < 8; ++j) {
    //         vhs.push_back(treemesh.add_vertex(c[j]));
    //     }
    //     treemesh.add_cell(vhs);
    // }
    // scene->addHexahedralMesh(treemesh);

    // Triangles
    std::vector<Rendering::Triangle> tris;
    for (uint i = 0; i < mesh.triangles.size(); ++i) {
        tris.emplace_back(
            mesh.vertices[mesh.triangles[i][0]],
            mesh.vertices[mesh.triangles[i][1]],
            mesh.vertices[mesh.triangles[i][2]],
            ui_color_ // TODO: Display correct color
            );
    }
    bbox_.compute(mesh.vertices);
    renderer_.addTriangles(tris, renderLoc_);
}

void ImplicitSurfaceObject::renderUIBody(Scene *scene)
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
    ImGui::SliderInt("Resolution", &resolution_, 2, 16);

    //-------------------
    // Update
    //-------------------
    if (ImGui::Button("Confirm")) {
        // Parse Input Text
        auto tokens = Parsing::tokenize(input_buffer_);
        //for (auto& token : tokens) std::cout << token << std::endl;

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

            return std::visit([](auto&& v) {
                using T = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<T, double>) return v;
                else {
                    std::cout << "Expected Scalar" << std::endl;
                    return 0.0;
                }
            }, value.val);
        };

        // Update renderer
        this->f_.f = func;
        renderer_.clear(renderLoc_);
        this->addToRenderer(scene);

        //std::cout << "f(1,2,3) = " << func(1, 2, 3) << std::endl;
    }
}

}
