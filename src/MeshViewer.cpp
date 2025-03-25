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

void MeshViewer::run()
{
    init();

    //for (const std::string& filename : {"../res/meshes/i25u.ovmb","../res/meshes/i01c.ovmb"})
    for (const std::string& filename : {"../res/meshes/i01c.ovmb"})
    {
        MV::TetrahedralMesh tetMesh;
        readMesh(filename, tetMesh, MV::FileFormat::OVMB);
        MV::TetMeshRenderer tetRenderer(tetMesh, camera);

        //auto where = renderer.addTetMesh(tetMesh);
    }

    //------------------------------------
    // Add some shapes for testing
    //------------------------------------

    // Triangle
    Point p0(glm::vec3{0,0,0}, glm::vec3{1,0,0});
    Point p1(glm::vec3{10,0,0}, glm::vec3{0,1,0});
    Point p2(glm::vec3{0,10,0}, glm::vec3{0,0,1});
    renderer.addPoint(p0);
    renderer.addPoint(p1);
    renderer.addPoint(p2);
    renderer.addTriangle(Triangle(p0, p1, p2));

    // Frame
    renderer.addFrame(glm::vec3(0,0,0), glm::vec3(50,0,0), glm::vec3(0,50,0), glm::vec3(0,0,50));

    // Sinus
    renderer.addExplicitCurve(Sin());

    // Circle
    std::vector<glm::vec3> circle;
    uint n = 100;
    float r = 20;
    for (uint i = 0; i < n; ++i)
    {
        circle.push_back(glm::vec3(
            r * std::cos(2.0 * M_PI * i / n),
            0,
            r * std::sin(2.0 * M_PI * i / n)
        ));
    }
    renderer.addConvexPolygon(true, circle, Color(1,1,0));
    renderer.addConvexPolygon(false, circle, Color(0,1,1));

    // Sphere
    renderer.addSphere(Vec3f(-100, -100, -100), 20, Color(1,0,0));

    // Tet
    renderer.addTetrahedron(Vec3f(10,10,10), Vec3f(20,10,10), Vec3f(10,10,0), Vec3f(10,20,10), Color(0,0,1));

    // Parametric Curve
    renderer.addParametricCurve([](float t) {return 100.f*Vec3f(2*cos(t)*cos(t),2*cos(t)*sin(t),sin(t));}, 0, 2*M_PI, Color(1,0,0));

    // Parametric Surface
    renderer.addParametricSurface([](float s, float t) {return Vec3f(s,30+s*s+t*t,t);}, Vec2f(-3,-3), Vec2f(3,3), Color(0,1,0));

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
    Renderer::RenderMatrices matrices(glm::mat4x4(1.0f), camera.getViewMatrix(), camera.getProjectionMatrix());

    // Picking
    if (MV::MouseHandler::LEFT_JUST_PRESSED)
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
        const glm::mat4x4 model_view_projection_matrix = projection_matrix * matrices.model_view_matrix;

        // Render to Picking Texture
        renderer.renderPicking(model_view_projection_matrix);

        // Read Pixel from Picking Texture
        int x = MV::MouseHandler::POSITION[0] * xscale / framebuffer_width * pickingTexture.getWidth();
        int y = MV::MouseHandler::POSITION[1] * yscale / framebuffer_height * pickingTexture.getHeight();
        picked = pickingTexture.readPixel(x, y);

        std::cout << "Picked: Batch: " << picked.batch_index << ", Buffer: " << picked.buffer_index << ", Primitive: " << picked.primitive_id << std::endl;

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

    // Render
    renderer.render(matrices);

    // ImGui - define and render
    MV::ImGuiRenderer::render(*this, renderer.settings);

    // Check for errors
    checkOpenGLError();

    // End of frame
    glfwSwapBuffers(window);
    MV::MouseHandler::update(window);
    MV::Time::update();
    camera.update(window);
    glfwPollEvents();
}

}
