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

void MeshViewer::addTetMesh(TetMeshRenderer& tmr)
{
    meshes.push_back(Mesh("Tet Mesh", std::make_shared<TetMeshRenderer>(tmr)));
}

void MeshViewer::addHexMesh(HexMeshRenderer& hmr)
{
    meshes.push_back(Mesh("Hex Mesh", std::make_shared<HexMeshRenderer>(hmr)));
}

void MeshViewer::run()
{
    init();

    // TODO: Currently, when we initialize a MeshRenderer this instance has to remain in scope. For example we cant create a temporary
    // variable and add it to a vector. Delete copy constructors/assignments of renderers/batches/buffers and implement move constructors/assignments

    MV::TetrahedralMesh tetMesh;
    readMesh("../res/meshes/i25u.ovmb", tetMesh, MV::FileFormat::OVMB);
    MV::TetMeshRenderer tetRenderer(tetMesh, camera);

    addTetMesh(tetRenderer);

    MV::Shader::FACES_OUTLINES_SHADER = MV::Shader("../res/shaders/outlines.glsl");
    MV::Shader::TET_CELLS_SHADER = MV::Shader("../res/shaders/cells.glsl");
    MV::Shader::FACES_SHADER = MV::Shader("../res/shaders/faces.glsl");
    MV::Shader::EDGES_SHADER = MV::Shader("../res/shaders/edges.glsl");
    MV::Shader::VERTICES_SHADER = MV::Shader("../res/shaders/vertices.glsl");

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
}

void MeshViewer::render()
{
    // Close Window by pressing ESCAPE
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

    if (MV::MouseHandler::LEFT_JUST_PRESSED)
    {
        //pickingTexture.bind();

        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // USE SHADER

        std::cout << "Click" << std::endl;

        //pickingTexture.unbind();

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
        mesh.renderer->render();
    }

    // ImGui - define and render
    MV::ImGuiRenderer::render(*this, meshes[0].renderer->settings);

    // Check for errors
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL Error: " << error << std::endl;
    }

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
