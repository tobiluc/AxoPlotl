#include "main.h"

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
Sphere sphere = Sphere(20);
Cube cube[] = { Cube(), Cube() };

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

    // build and compile our shader program
    Shader defaultShader("../res/shaders/default.vert", "../res/shaders/phong.frag");

    Texture texture_diffuse = Texture(), texture_specular = Texture(), texture_gen = Texture();

    texture_diffuse.generateFromFile("../res/textures/pebbles_diffuse.jpg");
    texture_specular.generateFromFile("../res/textures/pebbles_specular.jpg");

    texture_gen.generateProcedural(512, 512);

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

        defaultShader.use();
        texture_gen.use(GL_TEXTURE0);
        texture_specular.use(GL_TEXTURE1);

        glEnable(GL_DEPTH_TEST);

        mat4 model_matrix, view_matrix, modelview_matrix, projection_matrix, modelview_projection_matrix;
        mat3 normal_matrix;

        // Light positon
        float time = (float)glfwGetTime();
        vec3 light_position = vec3(3.f * cos(time*0.2f), 0.5f, 3.f * sin(time*0.2f));

        // Projection and View Matrix (Same for all objects)
        projection_matrix = perspective(radians(camera.fov), current_aspect_ratio, 0.1f, 100.0f);
        view_matrix = camera.getViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(defaultShader.ID, "view_matrix"), 1, GL_FALSE, value_ptr(view_matrix));

        defaultShader.setVec3("light.position", view_matrix * vec4(light_position, 1.f));
        defaultShader.setVec3("light.ambient", 0.1f, 0.1f, 0.1f);
        defaultShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
        defaultShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

        defaultShader.setInt("material.diffuse", 0);
        defaultShader.setInt("material.specular", 1);
        defaultShader.setFloat("material.shininess", 32.f);

        //=====================================================
        // DRAW ROTATING LIGHT CUBE
        //=====================================================

        // Model Matrix
        model_matrix = translate(mat4(1.0f), light_position);
        model_matrix = scale(model_matrix, vec3(0.1f));

        // Modelview Matrix
        modelview_matrix = view_matrix * model_matrix;

        // Modelview Projection Matrix
        modelview_projection_matrix = projection_matrix * modelview_matrix;

        // Normal Matrix
        normal_matrix = transpose(inverse(modelview_matrix));

        // Draw Cube
        glUniformMatrix4fv(glGetUniformLocation(defaultShader.ID, "modelview_projection_matrix"), 1, GL_FALSE, value_ptr(modelview_projection_matrix));
        glUniformMatrix3fv(glGetUniformLocation(defaultShader.ID, "normal_matrix"), 1, GL_FALSE, value_ptr(normal_matrix));
        cube[0].draw();

        //=====================================================
        // DRAW TEST CUBE
        //=====================================================
        // Model Matrix
        model_matrix = translate(mat4(1.0f), vec3(0.f, 5.f, 0.f));

        // Modelview Matrix
        modelview_matrix = view_matrix * model_matrix;

        // Modelview Projection Matrix
        modelview_projection_matrix = projection_matrix * modelview_matrix;

        // Normal Matrix
        normal_matrix = transpose(inverse(modelview_matrix));

        // Draw Cube
        glUniformMatrix4fv(glGetUniformLocation(defaultShader.ID, "modelview_projection_matrix"), 1, GL_FALSE, value_ptr(modelview_projection_matrix));
        glUniformMatrix3fv(glGetUniformLocation(defaultShader.ID, "normal_matrix"), 1, GL_FALSE, value_ptr(normal_matrix));
        cube[1].draw();

        //=====================================================
        // DRAW PLANET
        //=====================================================

        // Model Matrix
        model_matrix = mat4(1.f);

        // Modelview Matrix
        modelview_matrix = view_matrix * model_matrix;

        // Modelview Projection Matrix
        modelview_projection_matrix = projection_matrix * modelview_matrix;

        // Normal Matrix
        normal_matrix = transpose(inverse(modelview_matrix));

        // Draw Sphere
        glUniformMatrix4fv(glGetUniformLocation(defaultShader.ID, "modelview_projection_matrix"), 1, GL_FALSE, value_ptr(modelview_projection_matrix));
        glUniformMatrix3fv(glGetUniformLocation(defaultShader.ID, "normal_matrix"), 1, GL_FALSE, value_ptr(normal_matrix));
        sphere.draw();

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
        std::cout << "FPS: " << 1.0f / delta_time << "s" << std::endl;
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