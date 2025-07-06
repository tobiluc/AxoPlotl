#include <glad/glad.h>
#include "Scene.h"
#include "AxoPlotl/algorithms/marching_cubes.h"
#include "AxoPlotl/utils/FileAccessor.h"
#include "AxoPlotl/utils/Memory.h"
#include "ImGuiFileDialog.h"
#include "rendering/ImGuiRenderer.h"
#include <GLFW/glfw3.h>
#include "utils/Color.h"
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
    Renderer::RenderMatrices matrices(glm::mat4x4(1.0f), camera_.getViewMatrix(), camera_.getProjectionMatrix());

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
        GLuint clearColor[3] = {0,0,0};
        glClearBufferuiv(GL_COLOR, 0, clearColor);
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

        // Change Projection Matrix for Framebuffer (ratio might differ from viewport)
        const auto& projection_matrix = camera_.getProjectionMatrix(framebuffer_ratio);
        const glm::mat4x4 model_view_projection_matrix = projection_matrix * matrices.model_view_matrix;

        // Render to Picking Texture
        renderer_.renderPicking(model_view_projection_matrix);

        // Read Pixel from Picking Texture
        int x = AxoPlotl::MouseHandler::POSITION[0] * xscale / framebuffer_width * pickingTexture_.getWidth();
        int y = AxoPlotl::MouseHandler::POSITION[1] * yscale / framebuffer_height * pickingTexture_.getHeight();
        picked_ = pickingTexture_.readPixel(x, y);

        std::cout << "Picked: Batch: " << picked_.batch_index << ", Buffer: " << picked_.buffer_index << ", Primitive: " << picked_.primitive_id << std::endl;

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
    renderer_.render(matrices);

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

}

void Scene::renderUI()
{
    //-------------
    // Menu Bar
    //-------------
    if (ImGui::BeginMainMenuBar()) {
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
                    addExplicitSurface("Sphere", ExplictSurfaceFunctionBuilder::sphere(), Color::random());
                }

                if (ImGui::MenuItem("Torus")) {
                    addExplicitSurface("Torus", ExplictSurfaceFunctionBuilder::torus(), Color::random());
                }

                ImGui::EndMenu(); // !Explicit 2D Surface
            }

            if (ImGui::BeginMenu("Implicit 2D Surface")) {

                if (ImGui::MenuItem("Custom")) {

                }
                ImGui::Separator();

                if (ImGui::MenuItem("Sphere")) {

                }

                ImGui::EndMenu(); // !Explicit 2D Surface
            }

            if (ImGui::BeginMenu("Mesh")) {

                if (ImGui::MenuItem("Load from File")) {

                    IGFD::FileDialogConfig config;
                    config.path = "..";
                    ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".ovm,.ovmb", config);
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

            addTetrahedralMesh(filepath);
            //AxoPlotl::TetrahedralMesh mesh;
            //AxoPlotl::readMesh(filepath, mesh);
            //renderer_.addTetMesh(mesh);
        }
        ImGuiFileDialog::Instance()->Close();
    }

    // Begin
    ImGui::Begin("AxoPlotl Control Panel");

    // Define
    ImGui::Text("Info");
    ImGui::Text("%s", ("FPS " + std::to_string(Time::FRAMES_PER_SECOND)).c_str());
    ImGui::Text("%s", ("MEM " + std::to_string(getMemoryUsageMB()) + " MB").c_str());

    ImGui::NewLine();

    //---------------------------
    // Right Click on Geometry
    //---------------------------
    if (picked_.batch_index != 0 && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
    {
        ImGui::OpenPopup(("mesh_popup_" + std::to_string(picked_.batch_index)).c_str());
    }
    if (ImGui::BeginPopup(("mesh_popup_" + std::to_string(picked_.batch_index)).c_str()))
    {
        // Get Render Batch
        int i = picked_.batch_index - 1;

        // Modify Render Settings of Batch
        std::string str = "TEMP";
        ImGui::Text("%s", str.c_str());

        auto& settings = renderer_.settings;

        // Mesh Visibility
        ImGui::Checkbox("Show Cells", &settings.showCells);
        ImGui::Checkbox("Show Faces", &settings.showFaces);
        ImGui::Checkbox("Show Edges", &settings.showEdges);
        ImGui::Checkbox("Show Vertices", &settings.showVertices);

        // Material
        ImGui::NewLine();
        //ImGui::SliderFloat("Vertex Size", &settings.pointSize, 1.f, 16.0f);
        ImGui::SliderFloat("Edge Width", &settings.lineWidth, 1.f, 16.0f);
        ImGui::SliderFloat("Cell Scale", &settings.cellScale, 0.0f, 1.0f);
        ImGui::SliderFloat("Polygon Outline Width", &settings.outlineWidth, 0.0f, 12.0f);
        ImGui::ColorEdit3("Polygon Outline Color", &settings.outlineColor[0]);
        ImGui::Checkbox("Use Override Color", &settings.useColorOverride);
        ImGui::ColorEdit3("Override Color", &settings.colorOverride[0]);
        ImGui::ColorEdit3("Ambient", &settings.light.ambient[0]);
        ImGui::ColorEdit3("Diffuse", &settings.light.diffuse[0]);
        ImGui::ColorEdit3("Specular", &settings.light.specular[0]);

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
        objects_[i]->renderUI();
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
    Rendering::Renderer::GeometryLocation loc;

    // Triangle
    Rendering::Point p0(glm::vec3{0,0,0}, glm::vec3{1,0,0});
    Rendering::Point p1(glm::vec3{10,0,0}, glm::vec3{0,1,0});
    Rendering::Point p2(glm::vec3{0,10,0}, glm::vec3{0,0,1});
    renderer_.addPoint(p0, loc);
    renderer_.addPoint(p1, loc);
    renderer_.addPoint(p2, loc);
    renderer_.addTriangle(Rendering::Triangle(p0, p1, p2), loc);

    // Frame
    //renderer_.addFrame(glm::vec3(0,0,0), glm::vec3(50,0,0), glm::vec3(0,50,0), glm::vec3(0,0,50));

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

    // Sphere
    // renderer_.addSphere(Vec3f(50, 0, 0), 10, Color(1,0,0));
    // TriangleMesh mesh;
    // createTriangles(ExplictSurfaceFunctionBuilder::sphere(), mesh);
    // std::vector<Rendering::Triangle> tris;
    // for (uint i = 0; i < mesh.triangles.size(); ++i) {
    //     tris.emplace_back(
    //         Rendering::Point(mesh.vertices[mesh.triangles[i][0]],Color::BLUE),
    //         Rendering::Point(mesh.vertices[mesh.triangles[i][1]],Color::BLUE),
    //         Rendering::Point(mesh.vertices[mesh.triangles[i][2]],Color::BLUE)
    //         );
    // }
    // renderer_.addTriangles(tris, loc);

    // Spherical Harmonic
    // renderer_.addSphericalHarmonic([&](Vec3f p) {
    //     return pow(p.x,4) +  pow(p.y,4) +  pow(p.z,4);
    // }, 10);

    // Implicit Sphere
    // {
    // auto func = ImplicitSurfaceFunctionBuilder::heart();
    // TriangleMesh mesh;
    // createTriangles(func, mesh, 32);
    // std::vector<Rendering::Triangle> tris;
    // for (int i = 0; i < mesh.triangles.size(); ++i) {
    //     tris.emplace_back(
    //         Rendering::Point(mesh.vertices[mesh.triangles[i][0]], Color::GREEN),
    //         Rendering::Point(mesh.vertices[mesh.triangles[i][1]], Color::GREEN),
    //         Rendering::Point(mesh.vertices[mesh.triangles[i][2]], Color::GREEN)
    //         );
    // }
    // std::cout << tris.size() << " Triangles" << std::endl;
    // renderer_.addTriangles(tris, loc);
    // }

    // Torus
    // renderer_.addTorus(Vec3f(100, 0, 0), Vec3f(0,1,0), 4, 10, Color::BLUE);

    // Tet
    // renderer_.addTetrahedron(Vec3f(10,10,10), Vec3f(20,10,10), Vec3f(10,10,0), Vec3f(10,20,10), Color(0,0,1));

    // Parametric Curve
    // renderer_.addParametricCurve([](float t) {return 100.f*Vec3f(2*cos(t)*cos(t),2*cos(t)*sin(t),sin(t));}, 0, 2*M_PI, Color(1,0,0));

    // Parametric Surface
    // renderer_.addParametricSurface([](float s, float t) {return Vec3f(s,30+s*s+t*t,t);}, Vec2f(-3,-3), Vec2f(3,3),
                                   // Color(0,1,0));

    // Terrain
    // renderer_.addParametricSurface([&](float s, float t) {
    //     return Vec3f(10*s,-100+SimplexNoise::noise(s, t),10*t);
    // }, Vec2f(-10,-10), Vec2f(10,10), Color::WHITE, 128);


    //-----------------------
    // Init Input Field
    //-----------------------
    // inputs_.clear();
    // inputs_.push_back(AxPlInput(rootScope));
    // inputs_.push_back(AxPlInput(rootScope));
}

}
