#include <glad/glad.h>
#include "MeshViewer.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "rendering/ImGuiRenderer.h"
#include <GLFW/glfw3.h>
#include "utils/FileAccessor.h"
#include "utils/Globals.h"
#include "utils/MouseHandler.h"

namespace MV
{

void checkOpenGLError()
{
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL Error: " << error << std::endl;
    }
}

MeshViewer::MeshViewer() :
    window(nullptr),
    camera(Vec3f(0,0,30), Vec3f(0,0,-1)),
    clearColor(1,1,1)
{
}

MeshViewer::~MeshViewer()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}

void MeshViewer::addTetMesh(const std::string& name, TetMeshRenderer& tmr)
{
    meshes.emplace_back(name, std::make_shared<TetMeshRenderer>(tmr));
}

// void MeshViewer::addHexMesh(HexMeshRenderer& hmr)
// {
//     meshes.emplace_back("Hex Mesh", std::make_shared<HexMeshRenderer>(hmr));
// }

void MeshViewer::deleteMesh(size_t i)
{
    auto& mesh = meshes[i];
    mesh.renderer->deleteBuffers();
    meshes.erase(meshes.begin() + i);
}

void MeshViewer::run()
{
    init();

    for (const std::string& filename : {"../res/meshes/i25u.ovmb","../res/meshes/i01c.ovmb"})
    {
        MV::TetrahedralMesh tetMesh;
        readMesh(filename, tetMesh, MV::FileFormat::OVMB);
        MV::TetMeshRenderer tetRenderer(tetMesh, camera);

        addTetMesh(filename, tetRenderer);
    }

    //------------------------------------
    // Add some shapes for testing
    //------------------------------------
    TrianglesRenderBatch tr(1000);
    triangles.emplace_back(std::make_unique<TrianglesRenderBatch>(tr));
    triangles[0]->add(Triangle(
            Point(glm::vec3{0,0,0}, glm::vec3{1,0,0}),
            Point(glm::vec3{10,0,0}, glm::vec3{0,1,0}),
            Point(glm::vec3{0,10,0}, glm::vec3{0,0,1})));
    triangles[0]->add(Triangle(
           Point(glm::vec3{10,10,0}, glm::vec3{1,0,0}),
           Point(glm::vec3{0,10,0}, glm::vec3{0,1,0}),
            Point(glm::vec3{10,0,0}, glm::vec3{0,0,1})));

    // PolygonRenderer<4> qr(1000);
    // quads.emplace_back(std::make_unique<PolygonRenderer<4>>(qr));
    // quads[0]->add(Quad{.vertices = {
    //         Point{.position = {0,0,0}, .color={1,0,0}},
    //         Point{.position = {10,0,0}, .color={0,1,0}},
    //         Point{.position = {10,10,0}, .color={0,0,1}},
    //         Point{.position = {0,10,0}, .color={1,1,0}}
    //     }, .normal = {0, 0, 1}
    // });

    //-----------------------
    // Set Global Shaders
    //-----------------------
    MV::Shader::FACES_OUTLINES_SHADER = MV::Shader("../res/shaders/outlines.glsl");
    MV::Shader::TET_CELLS_SHADER = MV::Shader("../res/shaders/cells.glsl");
    MV::Shader::FACES_SHADER = MV::Shader("../res/shaders/faces.glsl");
    MV::Shader::EDGES_SHADER = MV::Shader("../res/shaders/edges.glsl");
    MV::Shader::VERTICES_SHADER = MV::Shader("../res/shaders/vertices.glsl");
    MV::Shader::PICKING_SHADER = MV::Shader("../res/shaders/picking.glsl");

    //--------------------
    // Main Render Loop
    //--------------------
    while (!glfwWindowShouldClose(window))
    {
        render();
    }
}

void MeshViewer::init()
{
    //--------------------------------
    // glfw: initialize and configure
    //--------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // set opengl version 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    //-----------------------
    // glfw window creation
    //-----------------------
    window = glfwCreateWindow(800, 600, "MVOMVO - My Very Own Mesh Viewer Option", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(1);
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, MV::framebuffer_size_callback);
    glfwSwapInterval(1); // <- Without this as many frames as possible are rendered and the GPU suffers

    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, MV::mouse_callback);
    glfwSetScrollCallback(window, MV::scroll_callback);

    //------------------------------------------
    // glad: load all OpenGL function pointers
    //------------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        exit(1);
    }
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    //------------------------------------------
    // ImGui
    //------------------------------------------
    MV::ImGuiRenderer::init(window);

    pickingTexture = PickingTexture(800, 600);
}

void MeshViewer::render()
{
    // Close Window by pressing ESCAPE
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

    // Cache matrices for rendering
    const glm::mat4x4 model_matrix(1.0f);
    const auto& view_matrix = camera.getViewMatrix();
    const auto& projection_matrix = camera.getProjectionMatrix();
    const glm::mat4x4 model_view_matrix = view_matrix * model_matrix;
    const glm::mat4x4 model_view_projection_matrix = projection_matrix * model_view_matrix;
    const glm::mat3x3 normal_matrix = glm::transpose(glm::inverse(model_view_matrix));

    // Picking
    if (MV::MouseHandler::LEFT_JUST_PRESSED && meshes.size() > 0)
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
        glViewport(0, 0, pickingTexture.getWidth(), pickingTexture.getHeight());

        // Bind Texture
        pickingTexture.bind();

        // Clear Texture
        GLuint clearColor[3] = {0,0,0};
        glClearBufferuiv(GL_COLOR, 0, clearColor);
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

        // Change Projection Matrix for Framebuffer (ratio might differ from viewport)
        const auto& projection_matrix = camera.getProjectionMatrix(framebuffer_ratio);
        const glm::mat4x4 model_view_projection_matrix = projection_matrix * model_view_matrix;

        // Render Meshes to Picking Texture
        for (int i = 0; i < meshes.size(); ++i)
        {
            meshes[i].renderer->renderPicking(model_view_projection_matrix, i);
        }

        // Read Pixel from Picking Texture
        int x = MV::MouseHandler::POSITION[0] * xscale / framebuffer_width * pickingTexture.getWidth();
        int y = MV::MouseHandler::POSITION[1] * yscale / framebuffer_height * pickingTexture.getHeight();
        picked = pickingTexture.readPixel(x, y);

        std::cout << "Picked: Mesh: " << picked.mesh_index << ", Element: " << picked.element_index << ", Primitive: " << picked.primitive_id << std::endl;

        // Unbind Texture
        pickingTexture.unbind();

        // Reset Viewport
        glViewport(0, 0, viewport_width, viewport_height);
    }

    // Clear the Screen Colors and Depth Buffer
    glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

    if (glfwGetCurrentContext() != window)
    {
        std::cerr << "OpenGL context is not active in MeshViewer!" << std::endl;
        exit(1);
    }

    // ImGui - declare new frame
    MV::ImGuiRenderer::newFrame();

    // Cache Viewport Size
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    float width = viewport[2];
    float height = viewport[3];

    // Render Meshes
    for (auto& mesh : meshes)
    {
        mesh.renderer->render(*this);
    }

    // Render Triangles
    Shader::FACES_SHADER.use();

    Shader::FACES_SHADER.setMat4x4f("view_matrix", view_matrix);
    Shader::FACES_SHADER.setMat4x4f("model_view_projection_matrix", model_view_projection_matrix);
    Shader::FACES_SHADER.setMat3x3f("normal_matrix", normal_matrix);

    Shader::FACES_SHADER.setVec3f("light.position", Vec3f(0,0,0));
    Shader::FACES_SHADER.setVec3f("light.ambient", Color{0.7f,0.7f,0.7f});
    Shader::FACES_SHADER.setVec3f("light.diffuse", Color{0.2f,0.2f,0.2f});
    Shader::FACES_SHADER.setVec3f("light.specular", Color{0.1f,0.1f,0.1f});

    Shader::FACES_OUTLINES_SHADER.use();

    Shader::FACES_OUTLINES_SHADER.setMat4x4f("model_view_projection_matrix", model_view_projection_matrix);
    Shader::FACES_OUTLINES_SHADER.setVec2f("inverse_viewport_size", 1.f/width, 1.f/height);
    Shader::FACES_OUTLINES_SHADER.setFloat("outline_width", 5);
    Shader::FACES_OUTLINES_SHADER.setVec3f("outline_color", Color{0.f,0.f,0.f});

    for (auto& tr : triangles)
    {
        tr->render();
    }
    // for (auto& qr : quads)
    // {
    //     qr->render();
    // }

    // ImGui - define and render
    MV::ImGuiRenderer::render(*this, meshes[0].renderer->settings);

    // Check for errors
    checkOpenGLError();

    // End of frame
    glfwSwapBuffers(window);
    MV::MouseHandler::update(window);
    MV::Time::update();
    camera.update(window);
    glfwPollEvents();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

}
