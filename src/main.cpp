#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "commons/Camera.h"
#include "rendering/ImGuiRenderer.h"
#include "rendering/TetMeshRenderer.h"
#include "utils/Globals.h"
#include "utils/Typedefs.h"
#include "utils/FileAccessor.h"

int main() {
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
    MV::WINDOW = glfwCreateWindow(800, 600, "MVOMVO - My Very Own Mesh Viewer Option", NULL, NULL);
    if (MV::WINDOW == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(MV::WINDOW);
    glfwSetFramebufferSizeCallback(MV::WINDOW, MV::Callbacks::framebuffer_size_callback);
    glfwSwapInterval(1); // <- Without this as many frames as possible are rendered and the GPU suffers

    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(MV::WINDOW, MV::Callbacks::mouse_callback);
    glfwSetScrollCallback(MV::WINDOW, MV::Callbacks::scroll_callback);

    //------------------------------------------
    // glad: load all OpenGL function pointers
    //------------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //------------------------------------------
    // Initialize Mesh Renderer
    //------------------------------------------
    std::cout << "Working in " << MV::getWorkingDirectory() << std::endl;
    MV::TetrahedralMesh mesh;
    MV::readTetMesh("../res/meshes/s17c.ovmb", mesh, MV::FileFormat::OVMB);
    MV::TetMeshRenderer tetRenderer(mesh);

    //------------------------------------------
    // ImGui
    //------------------------------------------
    MV::ImGuiRenderer::init();

    //--------------
    // render loop
    //--------------
    while (!glfwWindowShouldClose(MV::WINDOW)) {

        // Close Window by pressing ESCAPE
        if (glfwGetKey(MV::WINDOW, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(MV::WINDOW, true);

        // Move camera
        MV::CAMERA.processKeyboard();

        // Clear the Screen Colors and Depth Buffer
        glClearColor(MV::CLEAR_COLOR[0], MV::CLEAR_COLOR[1], MV::CLEAR_COLOR[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

        // ImGui - declare new frame
        MV::ImGuiRenderer::newFrame();

        // Mesh Renderer
        tetRenderer.render();

        // ImGui - define and render
        MV::ImGuiRenderer::render(tetRenderer);

        // Check for errors
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            std::cerr << "OpenGL Error: " << error << std::endl;
        }

        // End of frame
        glfwSwapBuffers(MV::WINDOW);
        glfwPollEvents();
        MV::Time::update();
    }

    MV::ImGuiRenderer::cleanup();
    glfwTerminate();
    return 0;
}
