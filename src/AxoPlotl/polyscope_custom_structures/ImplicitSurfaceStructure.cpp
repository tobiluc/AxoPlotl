#include "ImplicitSurfaceStructure.h"
#include "AxoPlotl/algorithms/parsing/reverse_polish.h"
#include "AxoPlotl/algorithms/parsing/tokens.h"
#include <AxoPlotl/algorithms/marching_cubes.h>

namespace polyscope
{

std::string implicit_surface_typename = "Implicit Surface";

ImplicitSurfaceStructure::ImplicitSurfaceStructure(
    const std::string& _name,
    const std::string& _input,
    const std::pair<float,float> _x_range,
    const std::pair<float,float> _y_range,
    const std::pair<float,float> _z_range,
    const uint32_t _resolution, const uint32_t _adaptive_depth)
    :
    QuantityStructure<ImplicitSurfaceStructure>(_name, implicit_surface_typename),
    x_range_(_x_range),
    y_range_(_y_range),
    z_range_(_z_range),
    resolution_(_resolution),
    adaptive_depth_(_adaptive_depth)
{
    regenerate(_input);
}

void ImplicitSurfaceStructure::buildCustomUI()
{
    // Input
    if (ImGui::InputText("##x", input_, sizeof(input_))) {
        regenerate();
    }
}

void ImplicitSurfaceStructure::buildCustomOptionsUI()
{
    // Range Options
    if (ImGui::SliderFloat2("x", &x_range_.first, -10, 10)) {
        regenerate();
    }
    if (ImGui::SliderFloat2("y", &y_range_.first, -10, 10)) {
        regenerate();
    }
    if (ImGui::SliderFloat2("z", &z_range_.first, -10, 10)) {
        regenerate();
    }

    // Resolution
    if (ImGui::SliderInt("Resolution", &resolution_, 1, 12)) {
        regenerate();
    }
    if (ImGui::SliderInt("Adaptive Depth", &adaptive_depth_, 0, 5)) {
        regenerate();
    }
}

void ImplicitSurfaceStructure::buildPickUI(const PickResult& result) {}

void ImplicitSurfaceStructure::buildQuantitiesUI() {}

void ImplicitSurfaceStructure::buildStructureOptionsUI() {}

void ImplicitSurfaceStructure::draw() {
    if (mesh_) {mesh_->draw();}
}

void ImplicitSurfaceStructure::drawDelayed() {
    if (mesh_) {mesh_->drawDelayed();}
}

void ImplicitSurfaceStructure::drawPick() {
    if (mesh_) {mesh_->drawPick();}
}

void ImplicitSurfaceStructure::drawPickDelayed() {
    if (mesh_) {mesh_->drawPickDelayed();}
}

void ImplicitSurfaceStructure::updateObjectSpaceBounds() {
    if (mesh_) {mesh_->updateObjectSpaceBounds();}
}

std::string ImplicitSurfaceStructure::typeName() {
    return implicit_surface_typename;
}

void ImplicitSurfaceStructure::refresh() {
    if (mesh_) {mesh_->refresh();}
}

void ImplicitSurfaceStructure::regenerate()
{
    // Parse Input Text
    auto tokens = AxoPlotl::Parsing::tokenize(input_);

    AxoPlotl::Parsing::RPN rpn;
    AxoPlotl::Parsing::reversePolish(tokens, rpn);
    AxoPlotl::Parsing::Variables vars;
    AxoPlotl::Parsing::Functions funcs;
    AxoPlotl::Parsing::registerCommons(vars, funcs);

    auto f = [&](const glm::vec3& v) {
        vars["x"] = v.x;
        vars["y"] = v.y;
        vars["z"] = v.z;
        return AxoPlotl::Parsing::evaluate(rpn, vars, funcs);
    };

    // Generate Mesh
    std::vector<glm::vec3> pts;
    std::vector<std::array<uint32_t,3>> tris;
    AxoPlotl::Algo::MarchingCubes::Settings cfg;
    cfg.resolution = {resolution_,resolution_,resolution_};
    cfg.max_adaptive_depth = adaptive_depth_;
    cfg.bounds.x0 = x_range_.first;
    cfg.bounds.x1 = x_range_.second;
    cfg.bounds.y0 = y_range_.first;
    cfg.bounds.y1 = y_range_.second;
    cfg.bounds.z0 = z_range_.first;
    cfg.bounds.z1 = z_range_.second;
    AxoPlotl::Algo::MarchingCubes::generateAdaptive(f, pts, tris, cfg);

    std::tuple<std::vector<uint32_t>, std::vector<uint32_t>> nestedListTup =
        standardizeNestedList<uint32_t,uint32_t,std::vector<std::array<uint32_t,3>>>(tris);
    std::vector<uint32_t>& faceIndsEntries = std::get<0>(nestedListTup);
    std::vector<uint32_t>& faceIndsStart = std::get<1>(nestedListTup);

    mesh_ = std::make_unique<SurfaceMesh>(name, pts, faceIndsEntries, faceIndsStart);
    objectSpaceBoundingBox = mesh_->boundingBox();
    objectSpaceLengthScale = mesh_->lengthScale();
}

void ImplicitSurfaceStructure::regenerate(const std::string& _input)
{
    std::strncpy(input_, _input.c_str(), sizeof(input_)-1);
    input_[sizeof(input_)-1] = '\0';
    regenerate();
}

Structure* ImplicitSurfaceStructure::setEnabled(bool newEnabled) {
    if (newEnabled == isEnabled()) {return this;}
    enabled = newEnabled;
    if (mesh_) {mesh_->setEnabled(newEnabled);}
    return this;
}

ImplicitSurfaceStructure* registerImplicitSurfaceStructure(
    const std::string& _name,
    const std::string& _input,
    const std::pair<float,float> _x_range,
    const std::pair<float,float> _y_range,
    const std::pair<float,float> _z_range,
    const uint32_t _resolution,
    const uint32_t _adaptive_depth)
{
    checkInitialized();

    auto* s = new polyscope::ImplicitSurfaceStructure(
        _name, _input,
        _x_range, _y_range, _z_range,
        _resolution, _adaptive_depth);
    bool success = polyscope::registerStructure(s);
    if (!success) {
        safeDelete(s);
        std::cerr << "Failed to register structure " << std::endl;
    }
    return s;
}

}
