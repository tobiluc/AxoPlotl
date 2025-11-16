#pragma once

#include "AxoPlotl/experimental/BoolQuantity.h"
#include "polyscope/curve_network.h"

namespace polyscope {

class EdgeBooleanQuantity : public CurveNetworkQuantity, public BooleanQuantity<EdgeBooleanQuantity> {
public:
    EdgeBooleanQuantity(const std::string& _name,
                        CurveNetwork& _network,
                        const std::vector<uint32_t>& _values_data);

    virtual void draw() override;
    virtual void buildCustomUI() override;
    virtual std::string niceName() override;
    virtual void refresh() override;

protected:
    // UI internals
    std::shared_ptr<render::ShaderProgram> program_;

    // Helpers
    virtual void createProgram() = 0;
};

} // namespace polyscope
