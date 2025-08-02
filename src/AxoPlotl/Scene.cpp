#include <glad/glad.h>
#include "Scene.h"
#include "AxoPlotl/Runner.h"
#include "AxoPlotl/IO/FileAccessor.h"
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
    glClearColor(clearColor_[0], clearColor_[1], clearColor_[2], 1.0f);
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
    gizmoRenderer_.render(GL::MeshRenderer::Matrices(glm::mat4(1.0), view_matrix, projection_matrix));

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
                saveToFile("/Users/tobiaskohler/Projects/MeshViewer/res/scene.sce");
            }
            if (ImGui::MenuItem("Load Scene")) {
                loadFromFile("/Users/tobiaskohler/Projects/MeshViewer/res/scene.sce");
            }
            ImGui::EndMenu(); // !File
        }

        if (ImGui::BeginMenu("Add")) {

            if (ImGui::BeginMenu("Simple")) {

                if (ImGui::MenuItem("Convex Polygon")) {
                    addConvexPolygon({Vec3f(0,0,0), Vec3f(1,0,0), Vec3f(0,1,0)}, "Triangle");
                }

                ImGui::EndMenu(); // !Simple
            }

            if (ImGui::BeginMenu("Curve")) {

                if (ImGui::MenuItem("Empty")) {
                    addExplicitCurve("Empty", ExplicitCurveFunctionBuilder::dummy(), Color::random());
                }
                ImGui::Separator();

                if (ImGui::MenuItem("Butterfly")) {
                    addExplicitCurve("Butterfly", ExplicitCurveFunctionBuilder::butterfly(), Color::random());
                }

                ImGui::EndMenu(); // !Curve
            }

            if (ImGui::BeginMenu("Surface")) {

                if (ImGui::BeginMenu("Explicit")) {

                    if (ImGui::MenuItem("Empty")) {
                        addExplicitSurface("Empty", ExplicitSurfaceFunctionBuilder::dummy(), Color::random());
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

                    ImGui::EndMenu(); // !Explicit
                }

                if (ImGui::BeginMenu("Implicit")) {

                    if (ImGui::MenuItem("Empty")) {
                        addImplicitSurface("Empty", ImplicitSurfaceFunctionBuilder::dummy());
                    }
                    ImGui::Separator();

                    if (ImGui::MenuItem("Sphere")) {
                        addImplicitSurface("Sphere", ImplicitSurfaceFunctionBuilder::sphere());
                    }

                    if (ImGui::MenuItem("Cube")) {
                        addImplicitSurface("Cube", ImplicitSurfaceFunctionBuilder::cube());
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

                    ImGui::EndMenu(); // !Implicit
                }

                ImGui::EndMenu(); // !Surface
            }

            if (ImGui::BeginMenu("Others")) {

                if (ImGui::MenuItem("Vector Field")) {
                    addVectorField([](const Vec3f& p) {
                        return p;
                    }, "Direction");
                }

                if (ImGui::MenuItem("Spherical Harmonic")) {
                    addSphericalHarmonic("SH", SphericalHarmonicFunctionBuilder::identityFrame());
                }

                ImGui::EndMenu();
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

        if (ImGui::BeginMenu("Edit")) {

            ImGui::EndMenu(); // !Edit
        }

        ImGui::EndMainMenuBar();
    }

    //-------------------------
    // Load Mesh Filed Dialog
    //-------------------------
    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
        if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
            std::string filepath = ImGuiFileDialog::Instance()->GetFilePathName();
            PolyhedralMesh mesh;
            if (IO::loadMesh(filepath, mesh)) {
                addMesh(mesh, std::filesystem::path(filepath).filename());
            } else {
                ImGui::BeginPopup("##Error");
                ImGui::Text("Could not load the mesh");
                ImGui::EndPopup();
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

    ImGui::Checkbox("Show Gizmos", &gizmoRenderer_.settings().visible);
    if (ImGui::Button("Reset Camera")) {
        camera_set_.reset(window);
    }

    ImGui::NewLine();

    //---------------------------
    // Right Click on Geometry
    //---------------------------
    if (picked_.object_index != 0 && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
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
        if (it != objects_.end())
        {
            ImGui::Text("Name: %s (Primitive %d)", it->get()->name(), picked_.primitive_id);
            if (ImGui::Button("Delete")) {
                it->get()->setDeleted();
            }
        }

        ImGui::EndPopup();
    }

    //---------------------
    // General Settings
    //---------------------
    ImGui::Checkbox("Orthographic", &camera_set_.use_ortho);
    ImGui::ColorEdit3("Background", &clearColor_[0]);

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

    // Coordinate Frame
    GL::MeshRenderer::Data data;
    data.lineAttribs.push_back(GL::MeshRenderer::VertexLineAttrib{.position=Vec3f(0,0,0),.color=Color::RED});
    data.lineAttribs.push_back(GL::MeshRenderer::VertexLineAttrib{.position=Vec3f(5,0,0),.color=Color::RED});
    data.lineAttribs.push_back(GL::MeshRenderer::VertexLineAttrib{.position=Vec3f(0,0,0),.color=Color::GREEN});
    data.lineAttribs.push_back(GL::MeshRenderer::VertexLineAttrib{.position=Vec3f(0,5,0),.color=Color::GREEN});
    data.lineAttribs.push_back(GL::MeshRenderer::VertexLineAttrib{.position=Vec3f(0,0,0),.color=Color::BLUE});
    data.lineAttribs.push_back(GL::MeshRenderer::VertexLineAttrib{.position=Vec3f(0,0,5),.color=Color::BLUE});
    for (uint i = 0; i < data.lineAttribs.size(); ++i) {data.lineIndices.push_back(i);}
    gizmoRenderer_.updateData(data);
    gizmoRenderer_.settings().useGlobalPointColor = false;
    gizmoRenderer_.settings().useGlobalLineColor = false;
    gizmoRenderer_.settings().useGlobalTriangleColor = false;
    gizmoRenderer_.settings().lineWidth = 8.0f;
    gizmoRenderer_.settings().pointSize = 12.0f;

    // PolyhedralMesh mesh;
    // IO::loadMesh("/Users/tobiaskohler/OF/OpenFlipper-Free/libs/libIGRec/res/output/IGREC_tet.ovmb", mesh);
    // addMesh(mesh, "Tet Mesh");

    // PolyhedralMesh mesh2;
    // IO::loadMesh("/Users/tobiaskohler/OF/OpenFlipper-Free/libs/libIGRec/res/output/IGREC_point_cloud.ovmb", mesh2);
    // addMesh(mesh2, "Point Cloud");
}

}
