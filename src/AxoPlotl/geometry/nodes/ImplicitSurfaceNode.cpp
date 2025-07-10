#include "ImplicitSurfaceNode.h"
#include "AxoPlotl/parsing/Scope.h"
#include "AxoPlotl/parsing/parsing.h"

namespace AxoPlotl
{

void ImplicitSurfaceNode::addToRenderer(Scene *scene)
{
    PolyhedralMesh mesh;
    TriangleMesh triangles;
    Octree tree;
    createTrianglesAMC(f_, triangles, tree, 16, octree_depth_);

    for (const auto& p : triangles.vertices) {
        mesh.add_vertex(toVec3<OVM::Vec3d>(p));
    }

    for (const auto& t : triangles.triangles) {
        mesh.add_face({OVM::VH(t[0]), OVM::VH(t[1]), OVM::VH(t[2])});
    }

    mesh_renderer_.init(mesh);
}

void ImplicitSurfaceNode::renderUIBody(Scene *scene)
{
    //-------------------
    // Text
    //-------------------
    ImGui::NewLine();
    ImGui::Text("f(x,y,z) = ");
    ImGui::SameLine();
    if (ImGui::InputText(" = 0", input_buffer_, sizeof(input_buffer_)))
    {
        // Text Changed
    }

    //-------------------
    // Range Options
    //-------------------
    ImGui::InputFloat2("x Range", &f_.xMin);
    ImGui::InputFloat2("y Range", &f_.yMin);
    ImGui::InputFloat2("z Range", &f_.zMin);
    ImGui::SliderInt("Octree Depth", &octree_depth_, 0, 4);

    //-------------------
    // Update
    //-------------------
    if (ImGui::Button("Confirm")) {
        // Parse Input Text
        auto tokens = Parsing::tokenize(input_buffer_);
        //for (auto& token : tokens) std::cout << token << std::endl;

        // Tranform AST into a function of x, y, z
        auto root = Parsing::Parser(tokens).parse();
        root->print();
        std::cout << std::endl;
        auto compiled = root->compile();
        auto func = [compiled](const Vec3f& v) {
            Scope scope;
            scope.setVariable("x", std::make_shared<Parsing::ScalarNode>(v.x));
            scope.setVariable("y", std::make_shared<Parsing::ScalarNode>(v.y));
            scope.setVariable("z", std::make_shared<Parsing::ScalarNode>(v.z));
            return compiled(scope).value<double>();
        };

        // std::function<float(Vec3f)> func = [&root](const Vec3f& v) {
        //     Scope scope;
        //     scope.setVariable("x", std::make_shared<Parsing::ScalarNode>(v.x));
        //     scope.setVariable("y", std::make_shared<Parsing::ScalarNode>(v.y));
        //     scope.setVariable("z", std::make_shared<Parsing::ScalarNode>(v.z));
        //     auto value = root->evaluate(scope);

        //     return std::visit([](auto&& v) {
        //         using T = std::decay_t<decltype(v)>;
        //         if constexpr (std::is_same_v<T, double>) return v;
        //         else {
        //             std::cout << "Expected Scalar" << std::endl;
        //             return 0.0;
        //         }
        //     }, value.val);
        // };

        // Update renderer
        this->f_.f = func;
        this->addToRenderer(scene);

        //std::cout << "f(1,2,3) = " << func(1, 2, 3) << std::endl;
    }
}

}
