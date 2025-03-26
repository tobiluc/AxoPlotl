#ifndef IMGUIRENDERER_H
#define IMGUIRENDERER_H

#include "../utils/Typedefs.h"
//#include "GLFW/glfw3.h"
#include "../AxoPlotl.h"

namespace AxPl
{

class ImGuiRenderer
{
public:
    static bool IMGUI_FOCUS;

    static void init(GLFWwindow* window);

    static void newFrame();
    
    static void render(MeshViewer& mv, Renderer::RenderSettings& settings);

private:
};
}
#endif // IMGUIRENDERER_H
