#pragma once

#include "polyscope/surface_mesh.h"
#include <cstdint>
#include <polyscope/quantity.h>

namespace polyscope
{

class ExplicitSurfaceStructure : public polyscope::QuantityStructure<ExplicitSurfaceStructure>
{
public:
    ExplicitSurfaceStructure(
        const std::string& _name,
        const std::string& _input_x,
        const std::string& _input_y,
        const std::string& _input_z,
        const std::pair<float,float> _u_range,
        const std::pair<float,float> _v_range,
        const uint32_t _resolution);

    // Build the imgui display
    virtual void buildCustomUI() override;
    virtual void buildCustomOptionsUI() override;
    virtual void buildPickUI(const PickResult& result) override;

    virtual void buildQuantitiesUI() override;
    virtual void buildStructureOptionsUI() override;

    // Standard structure overrides
    virtual void draw() override;
    virtual void drawDelayed() override;
    virtual void drawPick() override;
    virtual void drawPickDelayed() override;
    virtual void updateObjectSpaceBounds() override;
    virtual std::string typeName() override;
    virtual void refresh() override;

    Structure* setEnabled(bool newEnabled) override;

private:
    static constexpr uint32_t INPUT_BUFFER_SIZE = 512;
    char input_x_[INPUT_BUFFER_SIZE];
    char input_y_[INPUT_BUFFER_SIZE];
    char input_z_[INPUT_BUFFER_SIZE];
    std::pair<float, float> u_range_ = {0, 1};
    std::pair<float, float> v_range_ = {0, 1};
    int resolution_ = 32;
    std::unique_ptr<SurfaceMesh> mesh_;

    void regenerate();

    void regenerate(const std::string& _input_x,
        const std::string& _input_y,
        const std::string& _input_z);
};

ExplicitSurfaceStructure* registerExplicitSurfaceStructure(
    const std::string& _name,
    const std::string& _input_x = "",
    const std::string& _input_y = "",
    const std::string& _input_z = "",
    const std::pair<float,float> _u_range = {0,1},
    const std::pair<float,float> _v_range = {0,1},
    const uint32_t _resolution = 32);

}
