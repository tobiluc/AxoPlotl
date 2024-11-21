#include "main.h"
#include "utils/Typedefs.h"
#include "utils/FileAccessor.h"
#include "rendering/RenderBatch.h"

using namespace glm;

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
float current_aspect_ratio = (float)SCR_WIDTH / (float)SCR_HEIGHT;

// global variables
float delta_time = 0.0f;
float sec_timer = 0.0f;
float last_frame = 0.0f;

bool wireframe = false;
bool wireframe_toggable = true;

Camera camera(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f));
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

    /***********************
    * TEST START
    ***********************/

    std::cout << "Working in " << MV::getWorkingDirectory() << std::endl;

    MV::TetrahedralMesh mesh;
    MV::readTetMesh("../res/meshes/i01c.ovmb", mesh, MV::FileFormat::OVMB);

    Shader shader("../res/shaders/color.vert", "../res/shaders/color_phong.frag");
    MV::RenderBatch batch = MV::RenderBatch(4*mesh.n_cells(), shader);
    batch.initialize();

    MV::Random random;

    // add faces of mesh to batch as triangles with random color
    // for (auto f_it = mesh.f_iter(); f_it.is_valid(); ++f_it)
    // {
    //     auto fh = *f_it;
    //     auto vhs = mesh.get_halfface_vertices(fh.halfface_handle(0));
    //     if (vhs.size()!=3 || !vhs[0].is_valid() || !vhs[1].is_valid() || !vhs[2].is_valid()) {std::cout << "Skip fh " << fh << std::endl; continue;}
    //     auto p0 = mesh.vertex(vhs[0]);
    //     auto p1 = mesh.vertex(vhs[1]);
    //     auto p2 = mesh.vertex(vhs[2]);
    //     MV::Color::Color col = {random.randf(), random.randf(), random.randf(), 1.0f};
    //     MV::VertexData v0 = MV::vertexData(p0, col, vec3(0, 0, 1));
    //     MV::VertexData v1 = MV::vertexData(p1, col, vec3(0, 0, 1));
    //     MV::VertexData v2 = MV::vertexData(p2, col, vec3(0, 0, 1));
    //     batch.addTriangle(v0, v1, v2);
    // }

    // add cells of mesh to batch as scaled down tets
    MV::Color::Color col = {0.4,0.8f,0.4,1};
    for (auto c_it = mesh.c_iter(); c_it.is_valid(); ++c_it)
    {
        auto ch = *c_it;
        auto vhs = mesh.get_cell_vertices(ch);
        std::unordered_map<MV::VertexHandle, MV::OVM::Vec3d> p;
        for (auto vh : vhs) p[vh] = mesh.vertex(vh);
        MV::scaleTetrahedron(p.at(vhs[0]), p.at(vhs[1]), p.at(vhs[2]), p.at(vhs[3]), 0.9);
        auto hfhs = mesh.cell(ch).halffaces();
        for (auto hfh : hfhs)
        {
            auto vhs3 = mesh.get_halfface_vertices(hfh);
            auto n = (p.at(vhs3[2]) - p.at(vhs3[0])).cross(p.at(vhs3[1]) - p.at(vhs3[0])).normalize();
            MV::VertexData v0 = MV::vertexData(p.at(vhs3[0]), col, n);
            MV::VertexData v1 = MV::vertexData(p.at(vhs3[1]), col, n);
            MV::VertexData v2 = MV::vertexData(p.at(vhs3[2]), col, n);
            batch.addTriangle(v0, v1, v2);
        }
    }

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

        /***********************
        * RENDER START
        ***********************/

        shader.use();

        glEnable(GL_DEPTH_TEST);

        mat4 model_matrix, view_matrix, modelview_matrix, projection_matrix, modelview_projection_matrix;
        mat3 normal_matrix;

        // Projection and View Matrix (Same for all objects)
        projection_matrix = perspective(radians(camera.fov), current_aspect_ratio, 0.1f, 4096.0f);
        view_matrix = camera.getViewMatrix();
        
        // Compute Model, View, Normal Matrices
        model_matrix = mat4x4(1.0f);
        modelview_matrix = view_matrix * model_matrix;
        modelview_projection_matrix = projection_matrix * modelview_matrix;
        normal_matrix = transpose(inverse(modelview_matrix));

        // Set Shader Uniforms
        shader.setFloat("time", glfwGetTime());
        shader.setMat4x4f("view_matrix", view_matrix);
        shader.setMat4x4f("modelview_projection_matrix", modelview_projection_matrix);
        shader.setMat3x3f("normal_matrix", normal_matrix);
        shader.setVec3f("light.position", camera.position);
        shader.setVec3f("light.ambient", MV::Vec3f(1,1,1));
        shader.setVec3f("light.diffuse", MV::Vec3f(0.1,0.1,0.1));
        shader.setVec3f("light.specular", MV::Vec3f(0.1,0.1,0.1));

        // Render Batch
        batch.render();

        /***********************
        * RENDER END
        ***********************/

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window) {

    float current_frame = (float)glfwGetTime();
    delta_time = current_frame - last_frame;
    last_frame = current_frame;
    sec_timer += delta_time;
    if (sec_timer >= 1.0f) {
        sec_timer -= 1.0f;
        std::cout << "FPS: " << 1.0f / delta_time << std::endl;
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
    vec3 direction = vec3();
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.processKeyboard(camera.FORWARD, delta_time);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.processKeyboard(camera.BACKWARD, delta_time);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.processKeyboard(camera.LEFT, delta_time);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.processKeyboard(camera.RIGHT, delta_time);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) camera.processKeyboard(camera.UP, delta_time);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) camera.processKeyboard(camera.DOWN, delta_time);
}

void mouse_callback(GLFWwindow* window, double mouse_x, double mouse_y) {

    float dx = (float)(mouse_x - last_mouse_x);
    float dy = (float)(last_mouse_y - mouse_y); // reversed since y-coordinates range from bottom to top
    last_mouse_x = (float)mouse_x;
    last_mouse_y = (float)mouse_y;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) return; // Rotate by dragging

    camera.processMouseMovement(dx, dy);
}

void scroll_callback(GLFWwindow* window, double dx, double dy) {
    camera.processMouseScroll((float)dy);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    current_aspect_ratio = (float)width / (float)height;
    glViewport(0, 0, width, height);
}
