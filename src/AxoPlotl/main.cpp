//#define GLFW_INCLUDE_NONE
//#define GL_SILENCE_DEPRECATION

//#include <AxoPlotl/Runner.h>
#include "AxoPlotl/algorithms/marching_cubes.h"
#include "AxoPlotl/geometry/implicit_functions.h"
#include "AxoPlotl/geometry/ovm.h"
#include "AxoPlotl/interface/request_explicit_function.h"
#include "GLFW/glfw3.h"
#include "polyscope/point_cloud.h"
#include "polyscope/polyscope.h"
#include "polyscope/surface_mesh.h"
#include <filesystem>
#include <AxoPlotl/IO/FileAccessor.h>

void add_mesh_ovm(const AxoPlotl::PolyhedralMesh& mesh, std::string name) {
    std::vector<std::array<uint32_t,3>> tris;
    tris.reserve(mesh.n_faces());
    for (auto f_it = mesh.f_iter(); f_it.is_valid(); ++f_it) {
        const auto& vhs = mesh.get_halfface_vertices(f_it->halfface_handle(0));
        for (uint32_t i = 1; i < vhs.size()-1; ++i) {
            tris.push_back({
                vhs[0].uidx(), vhs[i].uidx(), vhs[i+1].uidx()
            });
        }
    }
    if (tris.empty()) {
        polyscope::registerPointCloud(name, mesh.vertex_positions());
    } else {
        polyscope::registerSurfaceMesh(name, mesh.vertex_positions(), tris);
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
                            // TODO
                        }

                        if (ImGui::MenuItem("Torus")) {
                            // TODO
                        }

                        if (ImGui::MenuItem("Moebius Strip")) {
                            // TODO
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
                        //TODO
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

        AxoPlotl::Interface::request_input_xyz(test_in_x, test_in_y, test_in_z);
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
            add_mesh_ovm(mesh, std::filesystem::path(paths[i]).filename());
        }
    });


    std::vector<std::array<double,3>> points;
    points.push_back({0,0,0});

    polyscope::registerPointCloud("my points", points);
    //polyscope::registerSurfaceMesh("my mesh", meshVerts, meshFaces);
    //polyscope::getSurfaceMesh("my mesh")->addVertexScalarQuantity("my_scalar", scalarQuantity);
    //polyscope::getSurfaceMesh("my mesh")->addFaceVectorQuantity("my_vector", vectorQuantity);

    // View the point cloud and mesh we just registered in the 3D UI
    polyscope::show();
}
