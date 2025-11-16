#include "ExplicitSurfaceStructure.h"
#include "AxoPlotl/algorithms/parsing/reverse_polish.h"
#include "AxoPlotl/algorithms/parsing/tokens.h"

namespace polyscope
{

std::string explicit_surface_typename = "Explicit Surface";

ExplicitSurfaceStructure::ExplicitSurfaceStructure(
        const std::string& _name,
        const std::string& _input_x,
        const std::string& _input_y,
        const std::string& _input_z,
        const std::pair<float,float> _u_range,
        const std::pair<float,float> _v_range,
        const uint32_t _resolution)
    :
    QuantityStructure<ExplicitSurfaceStructure>(_name, explicit_surface_typename),
    u_range_(_u_range),
    v_range_(_v_range),
    resolution_(_resolution)
{
    regenerate(_input_x, _input_y, _input_z);
}

void ExplicitSurfaceStructure::buildCustomUI()
{
    // Input
    ImGui::NewLine();
    ImGui::Text("x(u,v) = ");
    ImGui::SameLine();
    if (ImGui::InputText("##x", input_x_, sizeof(input_x_))) {regenerate();} // x changed
    ImGui::Text("y(u,v) = ");
    ImGui::SameLine();
    if (ImGui::InputText("##y", input_y_, sizeof(input_y_))) {regenerate();}// y changed
    ImGui::Text("z(u,v) = ");
    ImGui::SameLine();
    if (ImGui::InputText("##z", input_z_, sizeof(input_z_))) {regenerate();}// z changed

}

void ExplicitSurfaceStructure::buildCustomOptionsUI()
{
    // Range Option
    if (ImGui::SliderFloat2("u", &u_range_.first, -10, 10)) {
        regenerate();
    }
    if (ImGui::SliderFloat2("v", &v_range_.first, -10, 10)) {
        regenerate();
    }

    // Resolution
    if (ImGui::SliderInt("Resolution", &resolution_, 1, 128)) {
        regenerate();
    }
}

void ExplicitSurfaceStructure::buildPickUI(const PickResult& result) {}

void ExplicitSurfaceStructure::buildQuantitiesUI() {}

void ExplicitSurfaceStructure::buildStructureOptionsUI() {}

void ExplicitSurfaceStructure::draw() {
    if (mesh_) {mesh_->draw();}
}

void ExplicitSurfaceStructure::drawDelayed() {
    if (mesh_) {mesh_->drawDelayed();}
}

void ExplicitSurfaceStructure::drawPick() {
    if (mesh_) {mesh_->drawPick();}
}

void ExplicitSurfaceStructure::drawPickDelayed() {
    if (mesh_) {mesh_->drawPickDelayed();}
}

void ExplicitSurfaceStructure::updateObjectSpaceBounds() {
    if (mesh_) {mesh_->updateObjectSpaceBounds();}
}

std::string ExplicitSurfaceStructure::typeName() {
    return explicit_surface_typename;
}

void ExplicitSurfaceStructure::refresh() {
    if (mesh_) {mesh_->refresh();}
}

void ExplicitSurfaceStructure::regenerate()
{
    std::cout << "Regenerate " << name << std::endl;

    // Parse Input Text
    auto tokens_x = AxoPlotl::Parsing::tokenize(input_x_);
    auto tokens_y = AxoPlotl::Parsing::tokenize(input_y_);
    auto tokens_z = AxoPlotl::Parsing::tokenize(input_z_);

    AxoPlotl::Parsing::RPN rpn_x;
    AxoPlotl::Parsing::reversePolish(tokens_x, rpn_x);
    AxoPlotl::Parsing::RPN rpn_y;
    AxoPlotl::Parsing::reversePolish(tokens_y, rpn_y);
    AxoPlotl::Parsing::RPN rpn_z;
    AxoPlotl::Parsing::reversePolish(tokens_z, rpn_z);

    auto f = [&](float u, float v) {
        AxoPlotl::Parsing::Variables vars;
        AxoPlotl::Parsing::Functions funcs;
        AxoPlotl::Parsing::registerCommons(vars, funcs);

        vars["u"] = u;
        vars["v"] = v;

        return glm::vec3(
            AxoPlotl::Parsing::evaluate(rpn_x, vars, funcs),
            AxoPlotl::Parsing::evaluate(rpn_y, vars, funcs),
            AxoPlotl::Parsing::evaluate(rpn_z, vars, funcs)
        );
    };

    // Generate Vertex Positions
    std::vector<glm::vec3> points;
    std::vector<std::array<float,2>> uvs;
    points.reserve((resolution_+1)*(resolution_+1));
    uvs.reserve((resolution_+1)*(resolution_+1));
    float s, t;
    for (int i = 0; i <= resolution_; ++i) {
        s = u_range_.first + i * (u_range_.second - u_range_.first) / resolution_;
        for (int j = 0; j <= resolution_; ++j) {
            t = v_range_.first + j * (v_range_.second - v_range_.first) / resolution_;
            points.push_back(f(s, t));
            //uvs.push_back({(float)i/resolution_, float(j)/resolution_});
            uvs.push_back({s,t});
        }
    }

    // Generate Quads
    std::vector<std::vector<size_t>> quads;
    quads.reserve((resolution_+1)*(resolution_+1));
    for (int i = 0; i < resolution_; ++i) {
        for (int j = 0; j < resolution_; ++j) {
            uint32_t row1 = i * (resolution_ + 1);
            uint32_t row2 = (i + 1) * (resolution_ + 1);

            quads.push_back({
                (row1 + j + 0),
                (row2 + j + 0),
                (row2 + j + 1),
                (row1 + j + 1),
            });
        }
    }

    mesh_ = std::make_unique<SurfaceMesh>(name, points, quads);
    mesh_->addVertexParameterizationQuantity("UV", uvs, ParamCoordsType::WORLD)->setEnabled(true);
    objectSpaceBoundingBox = mesh_->boundingBox();
    objectSpaceLengthScale = mesh_->lengthScale();
}

void ExplicitSurfaceStructure::regenerate(const std::string& _input_x,
                const std::string& _input_y,
                const std::string& _input_z)
{
    std::strncpy(input_x_, _input_x.c_str(), sizeof(input_x_)-1);
    input_x_[sizeof(input_x_)-1] = '\0';
    std::strncpy(input_y_, _input_y.c_str(), sizeof(input_y_)-1);
    input_y_[sizeof(input_y_)-1] = '\0';
    std::strncpy(input_z_, _input_z.c_str(), sizeof(input_z_)-1);
    input_z_[sizeof(input_z_)-1] = '\0';
    regenerate();
}

Structure* ExplicitSurfaceStructure::setEnabled(bool newEnabled) {
    if (newEnabled == isEnabled()) {return this;}
    enabled = newEnabled;
    if (mesh_) {mesh_->setEnabled(newEnabled);}
    return this;
}

ExplicitSurfaceStructure* registerExplicitSurfaceStructure(
    const std::string& _name,
    const std::string& _input_x,
    const std::string& _input_y,
    const std::string& _input_z,
    const std::pair<float,float> _u_range,
    const std::pair<float,float> _v_range,
    const uint32_t _resolution)
{
    checkInitialized();

    auto* s = new polyscope::ExplicitSurfaceStructure(
        _name, _input_x, _input_y, _input_z,
        _u_range, _v_range, _resolution);
    bool success = polyscope::registerStructure(s);
    if (!success) {
        safeDelete(s);
        std::cerr << "Failed to register structure " << std::endl;
    }
    return s;
}

}
