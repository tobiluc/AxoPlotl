#include "main.h"
#include "commons/Camera.h"
#include "rendering/TetMeshRenderer.h"
#include "utils/Typedefs.h"
#include "utils/FileAccessor.h"

//using namespace glm;

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
//float current_aspect_ratio = (float)SCR_WIDTH / (float)SCR_HEIGHT;

// global variables
float delta_time = 0.0f;
float fps = 0.0f;
float sec_timer = 0.0f;
float last_frame = 0.0f;

bool imguiFocus;

MV::Camera camera(MV::Vec3f(0.0f, 0.0f, 30.0f), MV::Vec3f(0.0f, 0.0f, -1.0f));

float last_mouse_x = 0.5f * SCR_WIDTH;
float last_mouse_y = 0.5f * SCR_HEIGHT;

int main() {
    //--------------------------------
    // glfw: initialize and configure
    //--------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    //-----------------------
    // glfw window creation
    //-----------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Mesh Viewer", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, MV::framebuffer_size_callback);
    glfwSwapInterval(1); // <- Without this as many frames as possible are rendered and the GPU suffers

    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    //------------------------------------------
    // glad: load all OpenGL function pointers
    //------------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //------------------------------------------
    // ImGui
    //------------------------------------------
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    /***********************
    * TEST START
    ***********************/

    std::cout << "Working in " << MV::getWorkingDirectory() << std::endl;

    MV::TetrahedralMesh mesh;
    MV::readTetMesh("../res/meshes/s17c.ovmb", mesh, MV::FileFormat::OVMB);
    MV::TetMeshRenderer tetRenderer(mesh);

    /***********************
    * TEST END
    ***********************/

    //--------------
    // render loop
    //--------------
    float model_scale[3] = {1.f,1.f,1.f};
    float clear_color[3] = {1.0f, 1.0f, 1.0f};
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        // Clear the Screen Colors and Depth Buffer
        glClearColor(clear_color[0], clear_color[1], clear_color[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        // ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        imguiFocus = (io.WantCaptureMouse || io.WantCaptureKeyboard);

        /***********************
        * CUSTOM RENDER START
        ***********************/

        glm::mat4x4 model_matrix(1.0f);
        for (char i = 0; i < 3; ++i) model_matrix[i][i] = model_scale[i];
        tetRenderer.render(model_matrix, camera.getViewMatrix(), camera.getProjectionMatrix());

        /***********************
        * CUSTOM RENDER END
        ***********************/

        // ImGui Window
        ImGui::Begin("Mesh Viewer Control Panel");

        ImGui::Text("Info");
        ImGui::Text("%s", ("FPS " + std::to_string(fps)).c_str());
        ImGui::NewLine();
        ImGui::Text("Mesh Visibility");
        ImGui::Checkbox("Show Cells", &tetRenderer.showCells);
        ImGui::Checkbox("Show Faces", &tetRenderer.showFaces);
        ImGui::Checkbox("Show Edges", &tetRenderer.showEdges);
        ImGui::ColorEdit3("Background Color", clear_color);
        ImGui::NewLine();
        ImGui::Text("Mesh Render Settings");
        ImGui::SliderFloat("Cell Scale", &tetRenderer.cellScale, 0.0f, 1.0f);
        ImGui::SliderFloat("Outline Width", &tetRenderer.outlineWidth, 0.0f, 0.1f);
        ImGui::ColorEdit3("Outline Color", &tetRenderer.outlineColor[0]);
        ImGui::SliderFloat3("Model Scale", model_scale, 0, 2);
        ImGui::NewLine();
        ImGui::Text("Camera and Lighting");
        ImGui::SliderFloat("fov", &camera.fov, 1.0f, 90.0f);
        ImGui::ColorEdit3("Ambient", &tetRenderer.light.ambient[0]);
        ImGui::ColorEdit3("Diffuse", &tetRenderer.light.diffuse[0]);
        ImGui::ColorEdit3("Specular", &tetRenderer.light.specular[0]);
        ImGui::End();

        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window)
{

    // Compute frames per second
    float current_frame = (float)glfwGetTime();
    delta_time = current_frame - last_frame;
    last_frame = current_frame;
    sec_timer += delta_time;
    if (sec_timer >= 1.0f) {
        sec_timer -= 1.0f;
        fps = 1.0f / delta_time;
    }

    // Close Window by pressing ESCAPE
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);


    // Move
    auto direction = MV::Vec3f();
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.processKeyboard(camera.FORWARD, delta_time);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.processKeyboard(camera.BACKWARD, delta_time);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.processKeyboard(camera.LEFT, delta_time);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.processKeyboard(camera.RIGHT, delta_time);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) camera.processKeyboard(camera.UP, delta_time);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) camera.processKeyboard(camera.DOWN, delta_time);
}

void mouse_callback(GLFWwindow* window, double mouse_x, double mouse_y)
{
    if (imguiFocus) return;

    float dx = (float)(mouse_x - last_mouse_x);
    float dy = (float)(last_mouse_y - mouse_y); // reversed since y-coordinates range from bottom to top
    last_mouse_x = (float)mouse_x;
    last_mouse_y = (float)mouse_y;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) return; // Rotate by dragging

    camera.processMouseMovement(dx, dy);
}

void scroll_callback(GLFWwindow* window, double dx, double dy) {
    if (imguiFocus) return;
    camera.processMouseScroll((float)dy);
}
