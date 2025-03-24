#ifndef IMGUIRENDERER_H
#define IMGUIRENDERER_H

#include "../utils/Typedefs.h"
//#include "GLFW/glfw3.h"
#include "../MeshViewer.h"

namespace MV
{

class ImGuiRenderer
{
public:
    static void init(GLFWwindow* window);

    static void newFrame();
    
    static void render(MeshViewer& mv, RenderSettings& settings);

private:
};
}
#endif // IMGUIRENDERER_H
