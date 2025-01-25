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

    // TODO: Currently, when we initialize a MeshRenderer this instance has to remain in scope. For example we cant create a temporary
    // variable and add it to a vector. Delete copy constructors/assignments of renderers/batches/buffers and implement move constructors/assignments

    for (const std::string& filename : {"../res/meshes/i25u.ovmb","../res/meshes/i01c.ovmb"})
    {
        MV::TetrahedralMesh tetMesh;
        readMesh(filename, tetMesh, MV::FileFormat::OVMB);
        MV::TetMeshRenderer tetRenderer(tetMesh, camera);

        addTetMesh(filename, tetRenderer);
    }

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

        glm::mat4x4 model_matrix(1.0f);
        const auto& view_matrix = camera.getViewMatrix();
        const auto& projection_matrix = camera.getProjectionMatrix(framebuffer_ratio);
        glm::mat4x4 model_view_matrix = view_matrix * model_matrix;
        glm::mat4x4 model_view_projection_matrix = projection_matrix * model_view_matrix;

        // Render Meshes to Picking Texture
        for (int i = 0; i < meshes.size(); ++i)
        {
            meshes[i].renderer->renderPicking(model_view_projection_matrix, i+1);
        }

        // Read Pixel from Picking Texture
        int x = MV::MouseHandler::POSITION[0] * xscale / framebuffer_width * pickingTexture.getWidth();
        int y = MV::MouseHandler::POSITION[1] * yscale / framebuffer_height * pickingTexture.getHeight();
        picked = pickingTexture.readPixel(x, y);

        // Unbind Texture
        pickingTexture.unbind();

        // Reset Viewport
        glViewport(0, 0, viewport_width, viewport_height);
    }

    // Clear the Screen Colors and Depth Buffer
    glClearColor(1, 1, 1, 1.0f);
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

    // Render Meshes
    for (auto& mesh : meshes)
    {
        mesh.renderer->render(*this);
    }

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
