//#define GLFW_INCLUDE_NONE
//#define GL_SILENCE_DEPRECATION

#include "AxoPlotl/algorithms/marching_cubes.h"
#include "AxoPlotl/commons/ovm.h"
#include "AxoPlotl/geometry/implicit_functions.h"
#include "AxoPlotl/interface/polyscope_register_ovm_mesh.h"
#include "AxoPlotl/interface/request_explicit_function.h"
#include "GLFW/glfw3.h"
#include "polyscope/point_cloud.h"
#include "polyscope/polyscope.h"
#include "polyscope/surface_mesh.h"
#include "polyscope/volume_mesh.h"
#include <filesystem>
#include <AxoPlotl/IO/FileAccessor.h>

void add_mesh_ovm(const std::string& name, const AxoPlotl::PolyhedralMesh& mesh) {
    if (mesh.n_cells() > 0) {
        AxoPlotl::Interface::register_volume_mesh(name, mesh);
    } else if (mesh.n_faces() > 0) {
        AxoPlotl::Interface::register_surface_mesh(name, mesh);
    } else if (mesh.n_edges() > 0) {
        AxoPlotl::Interface::register_curve_network(name, mesh);
    } else {
        AxoPlotl::Interface::register_point_cloud(name, mesh);
    }
};

int main()
{
    //AxoPlotl::Runner ap;
    //ap.run();

    polyscope::init();
    GLFWwindow* window = glfwGetCurrentContext();
    if (!window) {
        std::cerr << "No current GLFW context after polyscope::init()\n";
        return -1;
    }

    char test_in_x[512];
    char test_in_y[512];
    char test_in_z[512];

    auto add_implicit_function = [&](const AxoPlotl::Geometry::ImplicitSurfaceFunction& func, std::string name) {
        std::vector<glm::vec3> pts;
        std::vector<std::array<uint32_t,3>> tris;
        AxoPlotl::Algo::MarchingCubes::Settings cfg;
        cfg.bounds = func.bounds;
        AxoPlotl::Algo::MarchingCubes::generateAdaptive(func.f, pts, tris, cfg);
        polyscope::registerSurfaceMesh(name, pts, tris);
    };

    auto add_explicit_surface_function = [&](const std::string& name,
            const AxoPlotl::Geometry::ExplicitSurfaceFunction& func,
            const uint32_t resolution = 32) {
        std::vector<glm::vec3> pts;
        pts.reserve((resolution+1)*(resolution+1));
        std::vector<std::array<uint32_t,3>> tris;
        tris.reserve(2*(resolution+1)*(resolution+1));

        // Generate Vertex Positions
        float s, t;
        for (int i = 0; i <= resolution; ++i) {
            s = func.uMin + i * (func.uMax - func.uMin) / resolution;
            for (int j = 0; j <= resolution; ++j) {
                t = func.vMin + j * (func.vMax - func.vMin) / resolution;
                pts.push_back(func(s, t));
            }
        }

        // Generate Triangles
        for (int i = 0; i < resolution; ++i) {
            for (int j = 0; j < resolution; ++j) {
                uint32_t row1 = i * (resolution + 1);
                uint32_t row2 = (i + 1) * (resolution + 1);

                tris.push_back({
                    (row1 + j + 0),
                    (row2 + j + 0),
                    (row2 + j + 1)
                });

                tris.push_back({
                    (row2 + j + 1),
                    (row1 + j + 1),
                    (row1 + j + 0)
                });
            }
        }

        polyscope::registerSurfaceMesh(name, pts, tris);
    };

    polyscope::state::userCallback = [&]() {

        if (ImGui::BeginMainMenuBar()) {

            if (ImGui::BeginMenu("Add")) {

                if (ImGui::BeginMenu("Simple")) {

                    if (ImGui::MenuItem("Convex Polygon")) {
                        std::vector<glm::vec3> pts;
                        pts.push_back(glm::vec3{0,0,0});
                        pts.push_back(glm::vec3{1,0,0});
                        pts.push_back(glm::vec3{0,1,0});
                        std::vector<std::array<uint32_t,3>> tris;
                        tris.push_back({0,1,2});
                        polyscope::registerSurfaceMesh("Triangle", pts, tris);
                    }

                    ImGui::EndMenu(); // !Simple
                }

                if (ImGui::BeginMenu("Curve")) {

                    if (ImGui::MenuItem("Empty")) {
                        // TODO
                    }
                    ImGui::Separator();

                    if (ImGui::MenuItem("Butterfly")) {
                        // TODO
                    }

                    ImGui::EndMenu(); // !Curve
                }

                if (ImGui::BeginMenu("Surface")) {

                    if (ImGui::BeginMenu("Explicit")) {

                        if (ImGui::MenuItem("Empty")) {
                            // TODO
                        }
                        ImGui::Separator();

                        if (ImGui::MenuItem("Sphere")) {
                            add_explicit_surface_function("Sphere",
                                AxoPlotl::Geometry::ExplicitSurfaceFunctionBuilder::sphere());
                        }

                        if (ImGui::MenuItem("Torus")) {
                            add_explicit_surface_function("Sphere",
                                AxoPlotl::Geometry::ExplicitSurfaceFunctionBuilder::torus());
                        }

                        if (ImGui::MenuItem("Moebius Strip")) {
                            add_explicit_surface_function("Moebius Strip",
                                AxoPlotl::Geometry::ExplicitSurfaceFunctionBuilder::moebiusStrip());
                        }

                        ImGui::EndMenu(); // !Explicit
                    }

                    if (ImGui::BeginMenu("Implicit")) {

                        ImGui::Separator();

                        if (ImGui::MenuItem("Sphere")) {
                            add_implicit_function(
                                AxoPlotl::Geometry::ImplicitSurfaceFunctionBuilder::sphere(),
                                "Sphere");
                        }

                        if (ImGui::MenuItem("Cube")) {
                            add_implicit_function(
                                AxoPlotl::Geometry::ImplicitSurfaceFunctionBuilder::cube(),
                                "Cube");
                        }

                        if (ImGui::MenuItem("Torus")) {
                            add_implicit_function(
                                AxoPlotl::Geometry::ImplicitSurfaceFunctionBuilder::torus(),
                                "Torus");
                        }

                        if (ImGui::MenuItem("Gyroid")) {
                            add_implicit_function(
                                AxoPlotl::Geometry::ImplicitSurfaceFunctionBuilder::gyroid(),
                                "Gyroid");
                        }

                        if (ImGui::MenuItem("Heart")) {
                            add_implicit_function(
                                AxoPlotl::Geometry::ImplicitSurfaceFunctionBuilder::heart(),
                                "Heart");
                        }

                        if (ImGui::MenuItem("Wineglass")) {
                            add_implicit_function(
                                AxoPlotl::Geometry::ImplicitSurfaceFunctionBuilder::wineglass(),
                                "Wineglass");
                        }

                        if (ImGui::MenuItem("Test")) {
                            add_implicit_function(
                                AxoPlotl::Geometry::ImplicitSurfaceFunctionBuilder::test(),
                                "Test");
                        }

                        ImGui::EndMenu(); // !Implicit
                    }

                    ImGui::EndMenu(); // !Surface
                }

                if (ImGui::BeginMenu("Others")) {

                    if (ImGui::MenuItem("Vector Field")) {
                        // TODO
                    }

                    if (ImGui::MenuItem("Spherical Harmonic")) {
                        // TODO
                    }

                    ImGui::EndMenu(); // !Others
                }

                if (ImGui::BeginMenu("Mesh")) {

                    if (ImGui::MenuItem("Load from File")) {
                        // AxoPlotl::PolyhedralMesh mesh;
                        // std::filesystem::path path;
                        // if (AxoPlotl::IO::loadMesh(path, mesh)) {
                        //     add_mesh_ovm(mesh, path.filename());
                        // }
                    }
                    ImGui::EndMenu(); // !Mesh
                }

                ImGui::EndMenu(); // !Add
            }

            if (ImGui::BeginMenu("Edit")) {
                ImGui::EndMenu(); // !Edit
            }

            ImGui::EndMainMenuBar();
        }

        // --- your ImGui code here ---
        if (ImGui::Button("Say: Hello World")) {
            std::cout << "Hello World" << std::endl;
        }

        ImGui::Separator();
        ImGui::Text("Lorem Ipsum");
        static float val = 0.5f;
        ImGui::SliderFloat("Cool Slider", &val, 0.f, 1.f);

        std::vector<glm::vec3> points;
        std::vector<std::array<uint32_t,3>> triangles;
        if (AxoPlotl::Interface::request_input_xyz(test_in_x, test_in_y, test_in_z, points, triangles)) {
            polyscope::registerSurfaceMesh("TEST_EXPLICIT_SURFACE", points, triangles);
        }
    };

    //-----------------
    // Drop Files in
    //-----------------
    glfwSetDropCallback(window, [](GLFWwindow* window, int count, const char** paths) {

        for (int i = 0; i < count; ++i) {
            AxoPlotl::PolyhedralMesh mesh;
            AxoPlotl::IO::loadMesh(
                std::filesystem::path(std::filesystem::path(paths[i])),
                mesh);
            add_mesh_ovm(std::filesystem::path(paths[i]).filename(), mesh);
        }
    });

    // View the 3D UI
    polyscope::show();
}
