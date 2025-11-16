#include "AxoPlotl/commons/ovm.h"
#include "AxoPlotl/interface/imgui_file_dialog.h"
#include "AxoPlotl/polyscope_custom_structures/ExplicitSurfaceStructure.h"
#include "AxoPlotl/polyscope_custom_structures/ImplicitSurfaceStructure.h"
#include "GLFW/glfw3.h"
#include "polyscope/polyscope.h"
#include <filesystem>
#include <AxoPlotl/IO/FileAccessor.h>
#include <AxoPlotl/polyscope_custom_structures/OVMStructure.h>
#include <AxoPlotl/polyscope_custom_structures/PolyscopeOVM.h>

int main()
{
    // Init Polyscope
    polyscope::options::programName = "AxoPlotl";
    polyscope::options::verbosity = 1;
    polyscope::options::usePrefsFile = false;
    polyscope::options::autocenterStructures = false;
    polyscope::options::autoscaleStructures = false;
    polyscope::options::automaticallyComputeSceneExtents = true;
    polyscope::view::setNavigateStyle(polyscope::NavigateStyle::Turntable);

    polyscope::init();

    // polyscope::OVM::PolyhedralMesh tmp;
    // AxoPlotl::IO::loadMesh("/Users/tobiaskohler/Uni/IGRec/IGRec-Dataset/Output/IGRec_Domain_Volume.ovmb", tmp);
    // polyscope::register_polyscope_ovm("TEST", tmp);

    polyscope::state::userCallback = [&]() {

        if (ImGui::BeginMainMenuBar()) {

            if (ImGui::BeginMenu("Add")) {

                if (ImGui::BeginMenu("Surface")) {

                    if (ImGui::BeginMenu("Explicit")) {

                        if (ImGui::MenuItem("Empty")) {
                            polyscope::registerExplicitSurfaceStructure("Explicit Surface");
                        }
                        ImGui::Separator();

                        if (ImGui::MenuItem("Sphere")) {
                            polyscope::registerExplicitSurfaceStructure("Explicit Sphere",
                                "sin(u)*cos(v)", "cos(u)", "sin(u)*sin(v)",
                                {0, M_PI}, {0, 2*M_PI});
                        }

                        if (ImGui::MenuItem("Torus")) {
                            polyscope::registerExplicitSurfaceStructure("Explicit Torus",
                                "(2 + sin(u))*cos(v)", "(2 + sin(u))*sin(v)", "cos(u)",
                                {0, 2*M_PI}, {0, 2*M_PI});
                        }

                        if (ImGui::MenuItem("Moebius Strip")) {
                            polyscope::registerExplicitSurfaceStructure("Explicit Moebius Strip",
                                "(1 + 0.5*v*cos(0.5*u)) * cos(u)",
                                "(1 + 0.5*v*cos(0.5*u)) * sin(u)",
                                "0.5*v * sin(0.5*u)",
                                {0, 2*M_PI}, {-1,1});
                        }

                        ImGui::EndMenu(); // !Explicit
                    }

                    if (ImGui::BeginMenu("Implicit")) {

                        if (ImGui::MenuItem("Empty")) {
                            polyscope::registerImplicitSurfaceStructure("Implicit Surface");
                        }
                        ImGui::Separator();

                        if (ImGui::MenuItem("Sphere")) {
                            polyscope::registerImplicitSurfaceStructure("Implicit Sphere",
                            "x*x + y*y + z*z - 1",
                            {-2,2}, {-2,2}, {-2,2});
                        }

                        if (ImGui::MenuItem("Cube")) {
                            polyscope::registerImplicitSurfaceStructure("Implicit Cube",
                                "max(abs(x),abs(y),abs(z)) - 1",
                                {-2,2}, {-2,2}, {-2,2});
                        }

                        if (ImGui::MenuItem("Torus")) {
                            polyscope::registerImplicitSurfaceStructure("Implicit Torus",
                                "(2 - (x*x+z*z)^0.5)^2 + y^2 - 1",
                                {-2,2}, {-2,2}, {-2,2});
                        }

                        if (ImGui::MenuItem("Gyroid")) {
                            polyscope::registerImplicitSurfaceStructure("Implicit Gyroid",
                                "sin(x)*cos(y) + sin(y)*cos(z) + sin(z)*cos(x)",
                                {-10,10}, {-10,10}, {-10,10});
                        }

                        if (ImGui::MenuItem("Heart")) {
                            polyscope::registerImplicitSurfaceStructure("Implicit Heart",
                                "(x*x + (9.0/4.0)*z*z + y*y - 1)^3 - x*x*y*y*y - (9.0/80.0)*y*y*y*z*z",
                                {-2,2}, {-2,2}, {-2,2});
                        }

                        if (ImGui::MenuItem("Wineglass")) {
                            polyscope::registerImplicitSurfaceStructure("Implicit Wineglass",
                                "x^2 + z^2 - (ln(y + 3.2))^2 - 0.02",
                                {-2,2}, {-3,3}, {-2,2});
                        }

                        if (ImGui::MenuItem("Test")) {
                            polyscope::registerImplicitSurfaceStructure("Implicit Test",
                                "(x-2)*(x-2)*(x+2)*(x+2) + (y-2)*(y-2)*(y+2)*(y+2) + (z-2)*(z-2)*(z+2)*(z+2) + 3*(x*x*y*y + x*x*z*z + y*y*z*z) + 6*x*y*z - 10*(x*x+y*y+z*z) + 22",
                                {-10,10}, {-10,10}, {-10,10});
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
                        AxoPlotl::Interface::imgui_file_dialog_open("Mesh files (*.obj *.ovm *.ovmb *ply){.obj,.ovm,.ovmb,.ply}");
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

        auto load_path = AxoPlotl::Interface::imgui_file_dialog_get();
        if (load_path.has_value()) {
            AxoPlotl::PolyhedralMesh mesh;
            if (AxoPlotl::IO::loadMesh(load_path.value(), mesh)) {
                polyscope::registerOVMStructure(load_path.value().filename(), mesh);
            }
        }
    };

    // Get GLFW Window
    GLFWwindow* window = glfwGetCurrentContext();
    if (!window) {
        std::cerr << "No current GLFW context after polyscope::init()\n";
        return -1;
    }

    // File Dop In
    glfwSetDropCallback(window, [](GLFWwindow* window, int count, const char** paths) {

        for (int i = 0; i < count; ++i) {
            AxoPlotl::PolyhedralMesh mesh;
            AxoPlotl::IO::loadMesh(
                std::filesystem::path(std::filesystem::path(paths[i])),
                mesh);
            polyscope::registerOVMStructure(std::filesystem::path(paths[i]).filename(), mesh);
        }
    });

    // Main Loop
    while(!polyscope::windowRequestsClose())
    {
        polyscope::frameTick();
    }
}
