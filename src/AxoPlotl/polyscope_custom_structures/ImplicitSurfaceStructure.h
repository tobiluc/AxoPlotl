#pragma once

#include "polyscope/surface_mesh.h"
#include <cstdint>
#include <polyscope/quantity.h>

namespace polyscope
{

class ImplicitSurfaceStructure : public polyscope::QuantityStructure<ImplicitSurfaceStructure>
{
public:
    ImplicitSurfaceStructure(
        const std::string& _name,
        const std::string& _input,
        const std::pair<float,float> _x_range,
        const std::pair<float,float> _y_range,
        const std::pair<float,float> _z_range,
        const uint32_t _resolution,
        const uint32_t _adaptive_depth);

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
    char input_[INPUT_BUFFER_SIZE];
    std::pair<float, float> x_range_ = {0, 1};
    std::pair<float, float> y_range_ = {0, 1};
    std::pair<float, float> z_range_ = {0, 1};
    int resolution_ = 8;
    int adaptive_depth_ = 4;
    std::unique_ptr<SurfaceMesh> mesh_;

    void regenerate();

    void regenerate(const std::string& _input);
};

ImplicitSurfaceStructure* registerImplicitSurfaceStructure(
    const std::string& _name,
    const std::string& _input = "",
    const std::pair<float,float> _x_range = {0,1},
    const std::pair<float,float> _y_range = {0,1},
    const std::pair<float,float> _z_range = {0,1},
    const uint32_t _resolution = 8,
    const uint32_t _adaptive_depth = 4);

}
