#define GLFW_INCLUDE_NONE
#define GL_SILENCE_DEPRECATION

#include <AxoPlotl/Runner.h>

int main(int argc, char** argv)
{
    AxoPlotl::Runner ap;
    ap.run(argc, argv);
}
