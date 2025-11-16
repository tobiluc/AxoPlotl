#include "EdgeBoolQuantity.h"

#include "imgui.h"

namespace polyscope
{

EdgeBooleanQuantity::EdgeBooleanQuantity(
    const std::string& _name,
    CurveNetwork& _network,
    const std::vector<uint32_t>& _values_data)
    :
    CurveNetworkQuantity(_name, _network, true),
    BooleanQuantity(*this, _values_data)
{}

void EdgeBooleanQuantity::draw()
{
    if (!isEnabled()) return;

    if (program_ == nullptr) {
        createProgram();
    }

    // Set uniforms
    parent.setStructureUniforms(*program_);

    parent.setCurveNetworkEdgeUniforms(*program_);

    setBooleanUniforms(*program_);

    render::engine->setMaterialUniforms(*program_, parent.getMaterial());

    program_->draw();
}

void EdgeBooleanQuantity::buildCustomUI()
{
    ImGui::SameLine();

    // == Options popup
    if (ImGui::Button("Options")) {
        ImGui::OpenPopup("OptionsPopup");
    }
    if (ImGui::BeginPopup("OptionsPopup")) {

        buildBooleanOptionsUI();

        ImGui::EndPopup();
    }

    buildBooleanUI();
}

void EdgeBooleanQuantity::refresh()
{
    program_.reset();
    Quantity::refresh();
}

std::string EdgeBooleanQuantity::niceName()
{
    return name + " (boolean)";
}


void EdgeBooleanQuantity::createProgram()
{
    // Create the program to draw this quantity
    // TODO: Use custom shader                                        );
    program_ = render::engine->requestShader(
        "RAYCAST_CYLINDER",
        render::engine->addMaterialRules(parent.getMaterial(),
        {}));

    // Fill geometry buffers
    parent.fillEdgeGeometryBuffers(*program_);

    // Fill node color buffers
    program_->setAttribute("a_value", values_buffer_.getRenderAttributeBuffer());

    // Fill edge color buffers
    program_->setAttribute("a_value_tail", values_buffer_.getIndexedRenderAttributeBuffer(parent.edgeTailInds));
    program_->setAttribute("a_value_tip", values_buffer_.getIndexedRenderAttributeBuffer(parent.edgeTipInds));

    render::engine->setMaterial(*program_, parent.getMaterial());
}

} // namespace polyscope
