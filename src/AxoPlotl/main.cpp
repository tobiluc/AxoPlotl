//#define GLFW_INCLUDE_NONE
//#define GL_SILENCE_DEPRECATION

//#include <AxoPlotl/Runner.h>
#include "polyscope/polyscope.h"
#include "polyscope/surface_mesh.h"
#include "polyscope/point_cloud.h"

int main()
{
    //AxoPlotl::Runner ap;
    //ap.run();

    polyscope::init();

    std::vector<std::array<double,3>> points;
    points.push_back({0,0,0});

    polyscope::registerPointCloud("my points", points);
    //polyscope::registerSurfaceMesh("my mesh", meshVerts, meshFaces);
    //polyscope::getSurfaceMesh("my mesh")->addVertexScalarQuantity("my_scalar", scalarQuantity);
    //polyscope::getSurfaceMesh("my mesh")->addFaceVectorQuantity("my_vector", vectorQuantity);

    // View the point cloud and mesh we just registered in the 3D UI
    polyscope::show();
}
