#include <glad/glad.h>
#include <AxoPlotl/Runner.h>
#include <AxoPlotl/IO/FileAccessor.h>
#include <AxoPlotl/rendering/redraw_frames.h>
#include <AxoPlotl/utils/Time.h>
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "rendering/Shader.h"
#include "rendering/ImGuiRenderer.h"
#include <GLFW/glfw3.h>
#include <AxoPlotl/utils/MouseHandler.h>

namespace AxoPlotl
{
Runner::Runner() :
    window(nullptr),
    scene_()
{
}

Runner::~Runner()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}

static void checkOpenGLError()
{
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL Error: " << error << std::endl;
    }
}

void Runner::run()
{
    //-------------------------------------
    // Initialize OpenGL for Rendering
    //-------------------------------------
    init();

    //-------------------------------------
    // Prepare the Scene
    //-------------------------------------
    scene_.init();

    //-----------------------
    // Set Global Shaders
    //-----------------------
    Shader::loadGlobalShaders();

    //--------------------
    // Main Render Loop
    //--------------------
    Rendering::triggerRedraw();
    while (!glfwWindowShouldClose(window))
    {
        // Close Window by pressing ESCAPE
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

        // Update the Scene
        scene_.update(window);
        AxoPlotl::MouseHandler::update(window);
        AxoPlotl::Time::update();

        if (Rendering::redraw_frames_ > 0) {

            // Render the Scene
            scene_.render(window);
            glfwSwapBuffers(window);
            Rendering::redraw_frames_--;

            // Check for errors
            GLenum error = glGetError();
            if (error != GL_NO_ERROR) {
                std::cerr << "OpenGL Error: " << error << std::endl;
            }

        } else {
            glfwWaitEventsTimeout(0.01);
        }
    }
}

void Runner::init()
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
    window = glfwCreateWindow(800, 600, "AxoPlotl", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(1);
    }


    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow*,int width,int height) {
        glViewport(0, 0, width, height);
        Rendering::triggerRedraw();
    });
    glfwSwapInterval(1); // <- Without this as many frames as possible are rendered and the GPU suffers

    // Set this instance as the user of the window by storing a pointer to this
    glfwSetWindowUserPointer(window, this);

    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, AxoPlotl::mouse_callback);
    glfwSetScrollCallback(window, AxoPlotl::scroll_callback);
    glfwSetMouseButtonCallback(window, [](GLFWwindow*,int,int,int) {Rendering::triggerRedraw();});
    glfwSetKeyCallback(window, [](GLFWwindow*,int,int,int,int) {Rendering::triggerRedraw();});

    // Drop Callback -> Drag in Files to load them
    glfwSetDropCallback(window, [](GLFWwindow* window, int count, const char** paths) {

        Runner* app = static_cast<Runner*>(glfwGetWindowUserPointer(window));
        if (!app) {return;}

        for (int i = 0; i < count; ++i) {
            PolyhedralMesh mesh;
            if (IO::loadMesh(paths[i], mesh)) {
                app->scene_.addMesh(mesh, std::filesystem::path(paths[i]).filename());
            } else {
            }
        }
    });


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
    AxoPlotl::GL::ImGuiInit(window);
}

}
