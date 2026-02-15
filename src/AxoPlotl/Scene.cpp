#include <glad/glad.h>
#include "Scene.h"
#include "AxoPlotl/Runner.h"
#include "AxoPlotl/utils/Memory.h"
#include "ImGuiFileDialog.h"
#include "rendering/ImGuiRenderer.h"
#include <GLFW/glfw3.h>
#include "utils/Time.h"
#include "utils/MouseHandler.h"
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "AxoPlotl/geometry/nodes/SphericalHarmonicNode.h"
#include "AxoPlotl/geometry/nodes/VectorFieldNode.h"
#include "AxoPlotl/geometry/nodes/ConvexPolygonNode.h"
#include "AxoPlotl/geometry/nodes/ExplicitCurveNode.h"
#include "AxoPlotl/geometry/nodes/ExplicitSurfaceNode.h"
#include "AxoPlotl/geometry/Surface.h"
#include "AxoPlotl/geometry/nodes/ImplicitSurfaceNode.h"

namespace AxoPlotl
{

void Scene::init()
{
    pickingTexture_ = PickingTexture(800, 600);
}

void Scene::update(GLFWwindow* window)
{
    camera_set_.camera()->update(window);

    // Remove deleted objects
    objects_.erase(
        std::remove_if(objects_.begin(), objects_.end(), [&](const std::unique_ptr<GeometryNode>& obj) {
            return obj->isDeleted();
        }), objects_.end());
    objects_.shrink_to_fit();
}

void Scene::render(GLFWwindow *window)
{
    // Cache matrices for rendering
    glm::mat4 view_matrix = camera_set_.camera()->getViewMatrix();

    // Enable gl_ClipDistance[0/1/2]
    glEnable(GL_CLIP_DISTANCE0); // property data range
    glEnable(GL_CLIP_DISTANCE1); // x clip
    glEnable(GL_CLIP_DISTANCE2); // y clip
    glEnable(GL_CLIP_DISTANCE3); // z clip

    // Picking
    if (AxoPlotl::MouseHandler::LEFT_JUST_PRESSED || AxoPlotl::MouseHandler::RIGHT_JUST_PRESSED)
    {
        // Get Viewport and Framebuffer Size
        GLint viewport_width, viewport_height;
        getViewportSize(viewport_width, viewport_height);
        int framebuffer_width, framebuffer_height;
        glfwGetFramebufferSize(window, &framebuffer_width, &framebuffer_height);
        float framebuffer_ratio = (float)(framebuffer_width) / framebuffer_height;

        // Get Viewport Pixel Scale
        float xscale, yscale;
        glfwGetWindowContentScale(window, &xscale, &yscale);

        // Set Viewport Size to picking texture
        glViewport(0, 0, pickingTexture_.getWidth(), pickingTexture_.getHeight());

        // Bind Texture
        pickingTexture_.bind();

        // Clear Texture
        GLuint clearColor[3] = {UINT32_MAX,UINT32_MAX,UINT32_MAX};
        glClearBufferuiv(GL_COLOR, 0, clearColor);
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

        // Change Projection Matrix for Framebuffer (ratio might differ from viewport)
        const auto& projection_matrix = camera_set_.camera()->getProjectionMatrix(framebuffer_ratio);
        //const glm::mat4x4 model_view_projection_matrix = projection_matrix * matrices.model_view_matrix;

        // Render to Picking Texture
        for (uint i = 0; i < objects_.size(); ++i) {
            objects_[i]->renderPicking(view_matrix, projection_matrix);
        }
        //renderer_.renderPicking(model_view_projection_matrix);

        // Read Pixel from Picking Texture
        int x = AxoPlotl::MouseHandler::POSITION[0] * xscale / framebuffer_width * pickingTexture_.getWidth();
        int y = AxoPlotl::MouseHandler::POSITION[1] * yscale / framebuffer_height * pickingTexture_.getHeight();
        picked_ = pickingTexture_.readPixel(x, y);

        //std::cout << "Picked: Object: " << picked_.object_index << ", Buffer: " << picked_.buffer << ", Primitive: " << picked_.primitive_id << std::endl;

        // Unbind Texture
        pickingTexture_.unbind();

        // Reset Viewport
        glViewport(0, 0, viewport_width, viewport_height);
    }

    // Clear the Screen Colors and Depth Buffer
    glClearColor(clear_color_[0], clear_color_[1], clear_color_[2], 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

    if (glfwGetCurrentContext() != window)
    {
        std::cerr << "OpenGL context is not active in Scene!" << std::endl;
        exit(1);
    }

    // Render
    glm::mat4 projection_matrix = camera_set_.camera()->getProjectionMatrix();
    for (uint i = 0; i < objects_.size(); ++i) {
        objects_[i]->render(view_matrix, projection_matrix);
    }
    gizmo_renderer_.render(projection_matrix * view_matrix * glm::mat4(1.0));
    //gizmoRenderer_.render(GL::MeshRenderer::Matrices(glm::mat4(1.0), view_matrix, projection_matrix));

    // Render interface
    GL::ImGuiNewFrame();
    renderUI(window);
}

void Scene::renderUI(GLFWwindow *window)
{
    // Get Width and Height of Viewport
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    const float width = viewport[2];
    const float height = viewport[3];

    //-------------
    // Menu Bar
    //-------------
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {

            if (ImGui::MenuItem("Save Scene")) {
                IGFD::FileDialogConfig config;
                config.path = "..";
                ImGuiFileDialog::Instance()->OpenDialog("SaveViewDlgKey", "Choose File",
                                                        "(*.json){.json}", config);
            }

            if (ImGui::MenuItem("Load Scene")) {
                IGFD::FileDialogConfig config;
                config.path = "..";
                ImGuiFileDialog::Instance()->OpenDialog("LoadViewDlgKey", "Choose File",
                                                        "(*.json){.json}", config);
            }

            ImGui::EndMenu(); // !File
        }

        if (ImGui::BeginMenu("Add")) {

            if (ImGui::BeginMenu("Simple")) {

                if (ImGui::MenuItem("Convex Polygon")) {
                    add_object<ConvexPolygonNode>(std::vector<Vec3f>{Vec3f(0,0,0), Vec3f(1,0,0), Vec3f(0,1,0)}, "Triangle");
                }

                ImGui::EndMenu(); // !Simple
            }

            if (ImGui::BeginMenu("Curve")) {

                if (ImGui::MenuItem("Empty")) {
                    add_object<ExplicitCurveNode>("Empty", ExplicitCurveFunctionBuilder::dummy(), random_color());
                }
                ImGui::Separator();

                if (ImGui::MenuItem("Butterfly")) {
                    add_object<ExplicitCurveNode>("Butterfly", ExplicitCurveFunctionBuilder::butterfly(), random_color());
                }

                ImGui::EndMenu(); // !Curve
            }

            if (ImGui::BeginMenu("Surface")) {

                if (ImGui::BeginMenu("Explicit")) {

                    if (ImGui::MenuItem("Empty")) {
                        add_object<ExplicitSurfaceNode>("Empty", ExplicitSurfaceFunctionBuilder::dummy(), random_color());
                    }
                    ImGui::Separator();

                    if (ImGui::MenuItem("Sphere")) {
                        add_object<ExplicitSurfaceNode>("Sphere", ExplicitSurfaceFunctionBuilder::sphere(), random_color());
                    }

                    if (ImGui::MenuItem("Torus")) {
                        add_object<ExplicitSurfaceNode>("Torus", ExplicitSurfaceFunctionBuilder::torus(), random_color());
                    }

                    if (ImGui::MenuItem("Moebius Strip")) {
                        add_object<ExplicitSurfaceNode>("Moebius Strip", ExplicitSurfaceFunctionBuilder::moebiusStrip(), random_color());
                    }

                    ImGui::EndMenu(); // !Explicit
                }

                if (ImGui::BeginMenu("Implicit")) {

                    if (ImGui::MenuItem("Empty")) {
                        add_object<ImplicitSurfaceNode>("Empty", ImplicitSurfaceFunctionBuilder::dummy());
                    }
                    ImGui::Separator();

                    if (ImGui::MenuItem("Sphere")) {
                        add_object<ImplicitSurfaceNode>("Sphere", ImplicitSurfaceFunctionBuilder::sphere());
                    }

                    if (ImGui::MenuItem("Cube")) {
                        add_object<ImplicitSurfaceNode>("Cube", ImplicitSurfaceFunctionBuilder::cube());
                    }

                    if (ImGui::MenuItem("Torus")) {
                        add_object<ImplicitSurfaceNode>("Torus", ImplicitSurfaceFunctionBuilder::torus());
                    }

                    if (ImGui::MenuItem("Gyroid")) {
                        add_object<ImplicitSurfaceNode>("Gyroid", ImplicitSurfaceFunctionBuilder::gyroid());
                    }

                    if (ImGui::MenuItem("Heart")) {
                        add_object<ImplicitSurfaceNode>("Heart", ImplicitSurfaceFunctionBuilder::heart());
                    }

                    if (ImGui::MenuItem("Wineglass")) {
                        add_object<ImplicitSurfaceNode>("Wineglass", ImplicitSurfaceFunctionBuilder::wineglass());
                    }

                    if (ImGui::MenuItem("Test")) {
                        add_object<ImplicitSurfaceNode>("Test", ImplicitSurfaceFunctionBuilder::test());
                    }

                    ImGui::EndMenu(); // !Implicit
                }

                ImGui::EndMenu(); // !Surface
            }

            if (ImGui::BeginMenu("Others")) {

                if (ImGui::MenuItem("Vector Field")) {
                    add_object<VectorFieldNode>(GradientField([](const Vec3f& p) {
                        return p;
                    }), "Direction");
                }

                if (ImGui::MenuItem("Spherical Harmonic")) {
                    add_object<SphericalHarmonicNode>(SphericalHarmonicFunctionBuilder::identityFrame(), "SH");
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Mesh")) {

                if (ImGui::MenuItem("Load from File")) {

                    IGFD::FileDialogConfig config;
                    config.path = "..";
                    ImGuiFileDialog::Instance()->OpenDialog("LoadMeshDlgKey", "Choose File",
                    "Mesh files (*.obj *.ovm *.ovmb *ply){.obj,.ovm,.ovmb,.ply}", config);
                }

                ImGui::EndMenu(); // !Mesh
            }

            ImGui::EndMenu(); // !Add
        }

        if (ImGui::BeginMenu("Edit")) {

            ImGui::EndMenu(); // !Edit
        }

        if (ImGui::BeginMenu("Settings")) {

            ImGui::Checkbox("Auto-focus on new object", &auto_zoom_to_new_object_);

            if (ImGui::BeginMenu("Camera")) {

                if (ImGui::MenuItem("Reset")) {
                    camera_set_.reset(window);
                }

                ImGui::Checkbox("Use Orthographic View", &camera_set_.use_ortho);

                ImGui::SliderFloat("Sensitivity",
                    &camera_set_.camera()->sensitivity_, 0.0005f, 0.005f);

                ImGui::EndMenu(); // !Camera
            }

            ImGui::ColorEdit3("Background Color", &clear_color_[0]);

            ImGui::Checkbox("Show Gizmos", &gizmo_renderer_.render_anything_);

            ImGui::EndMenu(); // !View
        }

        ImGui::EndMainMenuBar();
    }

    // Save View File Dialog
    if (ImGuiFileDialog::Instance()->Display("SaveViewDlgKey")) {
        if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
            std::string filepath = ImGuiFileDialog::Instance()->GetFilePathName();
            //if (!IO::serialize(filepath, camera_set_)) {
            if (!saveToFile(filepath)) {
                ImGui::BeginPopup("##Error");
                ImGui::Text("Could not save the view");
                ImGui::EndPopup();
            }
        }
        ImGuiFileDialog::Instance()->Close();
    }

    // Load View File Dialog
    if (ImGuiFileDialog::Instance()->Display("LoadViewDlgKey")) {
        if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
            std::string filepath = ImGuiFileDialog::Instance()->GetFilePathName();
            //if (!IO::deserialize(filepath, camera_set_)) {
            if (!loadFromFile(filepath)) {
                ImGui::BeginPopup("##Error");
                ImGui::Text("Could not load the view");
                ImGui::EndPopup();
            }
        }
        ImGuiFileDialog::Instance()->Close();
    }

    // Load Mesh File Dialog
    if (ImGuiFileDialog::Instance()->Display("LoadMeshDlgKey")) {
        if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
            std::filesystem::path filepath = ImGuiFileDialog::Instance()->GetFilePathName();
            add_mesh(filepath);

            // } else {
            //     PolyhedralMesh mesh;
            //     if (IO::loadMesh(filepath, mesh)) {
            //         addMesh(mesh, std::filesystem::path(filepath).filename());
            //     } else {status = 1;}
            // }

            // if (status != 0) {
            //     ImGui::BeginPopup("##Error");
            //     ImGui::Text("Could not save the mesh");
            //     ImGui::EndPopup();
            // }
        }
        ImGuiFileDialog::Instance()->Close();
    }

    // Begin
    ImGui::Begin("AxoPlotl Control Panel");

    // Define
    ImGui::Text("%s, %s",
        ("FPS " + std::to_string((int)Time::FRAMES_PER_SECOND)).c_str(),
        ("MEM " + std::to_string((int)getMemoryUsageMB()) + " MB").c_str()
    );
    ImGui::NewLine();

    //---------------------------
    // Right Click on Geometry
    //---------------------------
    if (picked_.object_index > 0
        && picked_.object_index != -1
        && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
    {
        ImGui::OpenPopup(("object_popup_" + std::to_string(picked_.object_index)).c_str());
    }
    if (ImGui::BeginPopup(("object_popup_" + std::to_string(picked_.object_index)).c_str()))
    {
        // Find Object
        int id = picked_.object_index;
        auto it = std::find_if(objects_.begin(), objects_.end(), [id](const auto& obj) {
            return obj->id() == id;
        });
        if (it != objects_.end()) {
            it->get()->renderPickedUI(this, picked_);
        }
        ImGui::EndPopup();
    }

    //---------------------
    // Geometry Objects
    //---------------------
    ImGui::Separator();

    ImGui::BeginChild("ScrollRegion", ImVec2(0, 0.5f*ImGui::GetWindowHeight()), true, ImGuiWindowFlags_HorizontalScrollbar);
    for (uint i = 0; i < objects_.size(); ++i) {
        objects_[i]->renderUI(this);
        ImGui::Separator();
    }
    ImGui::EndChild();

    // End
    ImGui::End();

    // Render ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void TestScene::init()
{
    Scene::init();

    //for (const std::string& filename : {"../res/meshes/i25u.ovmb","../res/meshes/i01c.ovmb"})
    // for (const std::string& filename : {"../res/meshes/i01c.ovmb"})
    // {
    //     AxoPlotl::TetrahedralMesh tetMesh;
    //     readMesh(filename, tetMesh, IO::FileFormat::OVMB);

    //     //auto where = renderer.addTetMesh(tetMesh);
    // }

    //------------------------------------
    // Add some shapes for testing
    //------------------------------------
    VolumeMeshRenderer::StaticData gizmo_data;
    gizmo_data.n_edges_ = 3;
    gizmo_data.positions_.push_back(Vec4f(0,0,0,1));
    gizmo_data.positions_.push_back(Vec4f(1,0,0,1));
    gizmo_data.positions_.push_back(Vec4f(0,1,0,1));
    gizmo_data.positions_.push_back(Vec4f(0,0,1,1));
    gizmo_data.edge_draw_vertices_.push_back({.vertex_index=0,.edge_index=0});
    gizmo_data.edge_draw_vertices_.push_back({.vertex_index=1,.edge_index=0});
    gizmo_data.edge_draw_vertices_.push_back({.vertex_index=0,.edge_index=1});
    gizmo_data.edge_draw_vertices_.push_back({.vertex_index=2,.edge_index=1});
    gizmo_data.edge_draw_vertices_.push_back({.vertex_index=0,.edge_index=2});
    gizmo_data.edge_draw_vertices_.push_back({.vertex_index=3,.edge_index=2});
    gizmo_renderer_.init(gizmo_data);
    std::vector<VolumeMeshRenderer::EdgeData> gizmo_e_data;
    gizmo_e_data.push_back({.property = Vec4f(1,0,0,1)});
    gizmo_e_data.push_back({.property = Vec4f(0,1,0,1)});
    gizmo_e_data.push_back({.property = Vec4f(0,0,1,1)});
    gizmo_renderer_.update_edge_data(gizmo_e_data);

    // Coordinate Frame
    // GL::MeshRenderer::Data data;
    // data.lineAttribs.push_back(GL::MeshRenderer::VertexLineAttrib{.position=Vec3f(0,0,0),.color=Vec4f(1,0,0,1)});
    // data.lineAttribs.push_back(GL::MeshRenderer::VertexLineAttrib{.position=Vec3f(5,0,0),.color=Vec4f(1,0,0,1)});
    // data.lineAttribs.push_back(GL::MeshRenderer::VertexLineAttrib{.position=Vec3f(0,0,0),.color=Vec4f(0,1,0,1)});
    // data.lineAttribs.push_back(GL::MeshRenderer::VertexLineAttrib{.position=Vec3f(0,5,0),.color=Vec4f(0,1,0,1)});
    // data.lineAttribs.push_back(GL::MeshRenderer::VertexLineAttrib{.position=Vec3f(0,0,0),.color=Vec4f(0,0,1,1)});
    // data.lineAttribs.push_back(GL::MeshRenderer::VertexLineAttrib{.position=Vec3f(0,0,5),.color=Vec4f(0,0,1,1)});
    // for (uint i = 0; i < data.lineAttribs.size(); ++i) {data.lineIndices.push_back(i);}
    // gizmoRenderer_.updateData(data);
    // gizmoRenderer_.line_width_ = 8.0f;
    // gizmoRenderer_.point_size_ = 12.0f;

    // PolyhedralMesh mesh;
    // IO::loadMesh("/Users/tobiaskohler/OF/OpenFlipper-Free/libs/libIGRec/res/output/IGREC_tet.ovmb", mesh);
    // addMesh(mesh, "Tet Mesh");

    // PolyhedralMesh mesh2;
    // IO::loadMesh("/Users/tobiaskohler/OF/OpenFlipper-Free/libs/libIGRec/res/output/IGREC_point_cloud.ovmb", mesh2);
    // addMesh(mesh2, "Point Cloud");
}

bool Scene::saveToFile(const std::filesystem::path& filename) {
    return IO::save_json(filename, nlohmann::json(*this));
}

bool Scene::loadFromFile(const std::filesystem::path &filename) {
    nlohmann::json j;
    if (IO::load_json(filename, j)) {
        this->camera_set_ = std::move(j.at("camera_set_"));
        this->clear_color_ = std::move(j.at("clear_color_"));
        return true;
    }
    return false;
}

}
