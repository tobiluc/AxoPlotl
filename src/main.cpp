#include "main.h"
#include "commons/Camera.h"
#include "rendering/TetMeshRenderer.h"
#include "utils/Typedefs.h"
#include "utils/FileAccessor.h"

//using namespace glm;

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
float current_aspect_ratio = (float)SCR_WIDTH / (float)SCR_HEIGHT;

// global variables
float delta_time = 0.0f;
float sec_timer = 0.0f;
float last_frame = 0.0f;

bool imguiFocus;

bool wireframe = false;
bool wireframe_toggable = true;

MV::Camera camera(MV::Vec3f(0.0f, 0.0f, 30.0f), MV::Vec3f(0.0f, 0.0f, -1.0f));
//Sphere sphere = Sphere(20);
//MV::Triangle triangle = MV::Triangle(); 
//Cube cube[] = { Cube(), Cube() };

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
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
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
    MV::readTetMesh("../res/meshes/i01c.ovmb", mesh, MV::FileFormat::OVMB);

    //Shader shader("../res/shaders/color.vert", "../res/shaders/color_triangles_default.geom", "../res/shaders/color_phong.frag");
    MV::TetMeshRenderer tetRenderer(mesh);

    /***********************
    * TEST END
    ***********************/

    //--------------
    // render loop
    //--------------
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        // Clear the Screen Colors and Depth Buffer
        glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
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

        tetRenderer.render(camera.getViewMatrix(), glm::perspective(glm::radians(camera.fov), current_aspect_ratio, 0.1f, 4096.0f));

        /***********************
        * CUSTOM RENDER END
        ***********************/

        // ImGui Window
        ImGui::Begin("ImGui, Gui Window!");
        ImGui::Text("Fischers Fritz fischt frische Fische.");
        ImGui::Checkbox("Show Cells", &tetRenderer.showCells);
        ImGui::Checkbox("Show Faces", &tetRenderer.showFaces);
        ImGui::Checkbox("Show Edges", &tetRenderer.showEdges);
        ImGui::SliderFloat("Cell Scale", &tetRenderer.cellScale, 0.0f, 1.0f);
        ImGui::SliderFloat("fov", &camera.fov, 1.0f, 45.0f);
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

    float current_frame = (float)glfwGetTime();
    delta_time = current_frame - last_frame;
    last_frame = current_frame;
    sec_timer += delta_time;
    if (sec_timer >= 1.0f) {
        sec_timer -= 1.0f;
        std::cout << "FPS: " << 1.0f / delta_time << std::endl;
        std::cout << "Camera: pos (" << camera.position[0] << " " << camera.position[1] << " " << camera.position[2] << "), forward ("
                  << camera.forward[0] << " " << camera.forward[1] << " " << camera.forward[2] << ")" << std::endl;
    }

    // Close Window by pressing ESCAPE
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Toggle Wireframe Mode with Z Key (Note that glfw uses us keyboards)
    if (wireframe_toggable) {
        if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
            wireframe = !wireframe;
            glPolygonMode(GL_FRONT_AND_BACK, (wireframe) ? GL_LINE : GL_FILL);
            wireframe_toggable = false;
            std::cout << "Wireframe: " << ((wireframe)? "ON" : "OFF") << std::endl;
        }
    } else if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_RELEASE) {
        wireframe_toggable = true;
    }


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

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    current_aspect_ratio = (float)width / (float)height;
    glViewport(0, 0, width, height);
}
