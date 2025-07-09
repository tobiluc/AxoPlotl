#include <glad/glad.h>
#include "Scene.h"
#include "AxoPlotl/IO/OBJFileAccessor.h"
#include "AxoPlotl/geometry/Octree.h"
#include "AxoPlotl/IO/OVMFileAccessor.h"
#include "AxoPlotl/utils/Memory.h"
#include "ImGuiFileDialog.h"
#include "rendering/ImGuiRenderer.h"
#include <GLFW/glfw3.h>
#include "commons/Color.h"
#include "utils/Time.h"
#include "utils/MouseHandler.h"
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

namespace AxoPlotl
{

void Scene::init()
{
    pickingTexture_ = PickingTexture(800, 600);
}

void Scene::render(GLFWwindow *window)
{
    // Cache matrices for rendering
    //Renderer::RenderMatrices matrices(glm::mat4x4(1.0f), camera_.getViewMatrix(), camera_.getProjectionMatrix());
    glm::mat4 view_matrix = camera_.getViewMatrix();

    // Picking
    if (AxoPlotl::MouseHandler::LEFT_JUST_PRESSED)
    {
        // Get Viewport and Framebuffer Size
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        GLint viewport_width = viewport[2];
        GLint viewport_height = viewport[3];
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
        const auto& projection_matrix = camera_.getProjectionMatrix(framebuffer_ratio);
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

        std::cout << "Picked: Object: " << picked_.object_index << ", Buffer: " << picked_.buffer << ", Primitive: " << picked_.primitive_id << std::endl;

        // Unbind Texture
        pickingTexture_.unbind();

        // Reset Viewport
        glViewport(0, 0, viewport_width, viewport_height);
    }

    // Clear the Screen Colors and Depth Buffer
    glClearColor(clearColor_[0], clearColor_[1], clearColor_[2], 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

    if (glfwGetCurrentContext() != window)
    {
        std::cerr << "OpenGL context is not active in Scene!" << std::endl;
        exit(1);
    }

    // ImGui - declare new frame
    Rendering::ImGuiNewFrame();

    // Render
    glm::mat4 projection_matrix = camera_.getProjectionMatrix();
    for (uint i = 0; i < objects_.size(); ++i) {
        objects_[i]->render(view_matrix, projection_matrix);
    }
    gizmoRenderer_.render(Rendering::Renderer::RenderMatrices(glm::mat4(1.0), view_matrix, projection_matrix));

    // Render interface
    renderUI();

    // Check for errors
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL Error: " << error << std::endl;
    }

    // End of frame
    glfwSwapBuffers(window);
    AxoPlotl::MouseHandler::update(window);
    AxoPlotl::Time::update();
    camera_.update(window);
    glfwPollEvents();

    // Remove deleted objects
    objects_.erase(
        std::remove_if(objects_.begin(), objects_.end(), [&](const std::unique_ptr<AxPlGeometryObject>& obj) {
            return obj->isDeleted();
    }), objects_.end());

}

void Scene::renderUI()
{
    //-------------
    // Menu Bar
    //-------------
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Save Scene")) {
                saveScene("/Users/tobiaskohler/Projects/MeshViewer/res/scene.json");
            }
            if (ImGui::MenuItem("Load Scene")) {
                loadScene("/Users/tobiaskohler/Projects/MeshViewer/res/scene.json");
            }
            ImGui::EndMenu(); // !File
        }

        if (ImGui::BeginMenu("Add")) {

            if (ImGui::BeginMenu("Simple")) {

                if (ImGui::MenuItem("Point")) {

                }

                if (ImGui::MenuItem("Line")) {

                }

                if (ImGui::MenuItem("Triangle")) {

                }

                if (ImGui::MenuItem("Convex Polygon")) {

                }

                if (ImGui::MenuItem("Tetrahedron")) {

                }

                ImGui::EndMenu(); // !Simple
            }

            if (ImGui::BeginMenu("Explicit 1D Curve")) {
                ImGui::EndMenu(); // !Explicit 1D Curve
            }

            if (ImGui::BeginMenu("Explicit 2D Surface")) {

                if (ImGui::MenuItem("Custom")) {

                }
                ImGui::Separator();

                if (ImGui::MenuItem("Sphere")) {
                    addExplicitSurface("Sphere", ExplicitSurfaceFunctionBuilder::sphere(), Color::random());
                }

                if (ImGui::MenuItem("Torus")) {
                    addExplicitSurface("Torus", ExplicitSurfaceFunctionBuilder::torus(), Color::random());
                }

                if (ImGui::MenuItem("Moebius Strip")) {
                    addExplicitSurface("Moebius Strip", ExplicitSurfaceFunctionBuilder::moebiusStrip(), Color::random());
                }

                ImGui::EndMenu(); // !Explicit 2D Surface
            }

            if (ImGui::BeginMenu("Implicit 2D Surface")) {

                if (ImGui::MenuItem("Custom")) {
                    addImplicitSurface("Custom", ImplicitSurfaceFunctionBuilder::dummy());
                }
                ImGui::Separator();

                if (ImGui::MenuItem("Sphere")) {
                    addImplicitSurface("Sphere", ImplicitSurfaceFunctionBuilder::sphere());
                }

                if (ImGui::MenuItem("Torus")) {
                    addImplicitSurface("Torus", ImplicitSurfaceFunctionBuilder::torus());
                }

                if (ImGui::MenuItem("Gyroid")) {
                    addImplicitSurface("Gyroid", ImplicitSurfaceFunctionBuilder::gyroid());
                }

                if (ImGui::MenuItem("Heart")) {
                    addImplicitSurface("Heart", ImplicitSurfaceFunctionBuilder::heart());
                }

                if (ImGui::MenuItem("Wineglass")) {
                    addImplicitSurface("Wineglass", ImplicitSurfaceFunctionBuilder::wineglass());
                }

                if (ImGui::MenuItem("Test")) {
                    addImplicitSurface("Test", ImplicitSurfaceFunctionBuilder::test());
                }

                ImGui::EndMenu(); // !Explicit 2D Surface
            }

            if (ImGui::BeginMenu("Mesh")) {

                if (ImGui::MenuItem("Load from File")) {

                    IGFD::FileDialogConfig config;
                    config.path = "..";
                    ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File",
                    "Mesh files (*.obj *.ovm *.ovmb *ply){.obj,.ovm,.ovmb,.ply}", config);
                }

                ImGui::EndMenu(); // !Mesh
            }

            ImGui::EndMenu(); // !Add
        }
        ImGui::EndMainMenuBar();
    }

    //-------------------------
    // Load Mesh Filed Dialog
    //-------------------------
    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
        if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
            std::string filepath = ImGuiFileDialog::Instance()->GetFilePathName();

            auto format = IO::getFileFormatFromName(filepath);
            if (format == IO::FileFormat::OVMB || format == IO::FileFormat::OVM) {
                addTetrahedralMesh(filepath);
            } else {
                addMesh(filepath);
            }
        }
        ImGuiFileDialog::Instance()->Close();
    }

    // Begin
    ImGui::Begin("AxoPlotl Control Panel");

    // Define
    ImGui::Text("Info");
    ImGui::Text("%s", ("FPS " + std::to_string(Time::FRAMES_PER_SECOND)).c_str());
    ImGui::Text("%s", ("MEM " + std::to_string(getMemoryUsageMB()) + " MB").c_str());

    ImGui::Checkbox("Show Gizmos", &gizmoRenderer_.settings.visible);
    if (ImGui::Button("Reset Camera")) {
        camera_.setPosition(Vec3f(0,0,1));
        camera_.setForward(Vec3f(0,0,-1));
    }

    ImGui::NewLine();

    //---------------------------
    // Right Click on Geometry
    //---------------------------
    if (picked_.object_index != 0 && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
    {
        ImGui::OpenPopup(("mesh_popup_" + std::to_string(picked_.object_index)).c_str());
    }
    if (ImGui::BeginPopup(("mesh_popup_" + std::to_string(picked_.object_index)).c_str()))
    {
        // Get Render Batch
        int i = picked_.object_index - 1;

        // Modify Render Settings of Batch
        std::string str = "TEMP";
        ImGui::Text("%s", str.c_str());

        ImGui::EndPopup();
    }

    //---------------------
    // General Settings
    //---------------------
    ImGui::Checkbox("Orthographic", &camera_.isOrthographic);
    ImGui::ColorEdit3("Background", &clearColor_[0]);

    //---------------------
    // Geometry Objects
    //---------------------
    ImGui::Separator();

    for (uint i = 0; i < objects_.size(); ++i) {
        objects_[i]->renderUI(this);
        ImGui::Separator();
    }

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
    for (const std::string& filename : {"../res/meshes/i01c.ovmb"})
    {
        AxoPlotl::TetrahedralMesh tetMesh;
        readMesh(filename, tetMesh, IO::FileFormat::OVMB);

        //auto where = renderer.addTetMesh(tetMesh);
    }

    //------------------------------------
    // Add some shapes for testing
    //------------------------------------
    Rendering::Renderer::BatchIndices gizmoLoc;

    // Coordinate Frame
    gizmoRenderer_.addLines({
        Rendering::Line(Vec3f(0,0,0), Vec3f(5,0,0), Color::RED),
        Rendering::Line(Vec3f(0,0,0), Vec3f(0,5,0), Color::GREEN),
        Rendering::Line(Vec3f(0,0,0), Vec3f(0,0,5), Color::BLUE)
    }, gizmoLoc);
    gizmoRenderer_.settings.lineWidth = 5.0f;

    // Octree Test
    Octree tree(AABB{0,1,0,1,0,1});
    tree.refineNode(0);
    tree.refineNode(8);
    for (u32 i = 0; i < tree.numNodes(); ++i) {
        auto c = tree.getNodeBounds(i).center();
        std::cout << i << ": " << c[0] << " " << c[1] << " " << c[2] << std::endl;
    }
    HexahedralMesh mesh;
    for (u32 i = 0; i < tree.numNodes(); ++i) {
        auto c = tree.getNodeBounds(i).corners<OVM::Vec3d>();
        std::vector<OVM::VH> vhs;
        for (u32 j = 0; j < 8; ++j) {
            vhs.push_back(mesh.add_vertex(c[j]));
        }
        mesh.add_cell(vhs);
    }
    addHexahedralMesh(mesh);

    // Triangle
    // Rendering::Point p0(glm::vec3{0,0,0}, glm::vec3{1,0,0});
    // Rendering::Point p1(glm::vec3{10,0,0}, glm::vec3{0,1,0});
    // Rendering::Point p2(glm::vec3{0,10,0}, glm::vec3{0,0,1});
    // renderer_.addPoint(p0, loc);
    // renderer_.addPoint(p1, loc);
    // renderer_.addPoint(p2, loc);
    // renderer_.addTriangle(Rendering::Triangle(p0, p1, p2), loc);

    // Circle
    // std::vector<glm::vec3> circle;
    // uint n = 100;
    // float r = 20;
    // for (uint i = 0; i < n; ++i)
    // {
    //     circle.push_back(glm::vec3(
    //         r * std::cos(2.0 * M_PI * i / n),
    //         0,
    //         r * std::sin(2.0 * M_PI * i / n)
    //         ));
    // }
    // renderer_.addConvexPolygon(true, circle, Color(1,1,0));
    // renderer_.addConvexPolygon(false, circle, Color(0,1,1));

    // Spherical Harmonic
    // renderer_.addSphericalHarmonic([&](Vec3f p) {
    //     return pow(p.x,4) +  pow(p.y,4) +  pow(p.z,4);
    // }, 10);
}

}
