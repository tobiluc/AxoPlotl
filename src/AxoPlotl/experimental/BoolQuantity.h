#pragma once

#include "polyscope/persistent_value.h"
#include "polyscope/render/engine.h"
#include "polyscope/render/managed_buffer.h"

namespace polyscope {

// Encapsulates logic which is common to all boolean quantities

template <typename QuantityT>
class BooleanQuantity {
public:
    BooleanQuantity(QuantityT& quantity, const std::vector<uint32_t>& values);

    // Build the ImGUI UIs for booleans
    void buildBooleanUI();
    virtual void buildBooleanOptionsUI(); // called inside of an options menu

    // Set uniforms in rendering programs for scalars
    void setBooleanUniforms(render::ShaderProgram& p);

    template <class V>
    void updateData(const V& newValues);

    // === Members
    QuantityT& quantity;

    // Wrapper around the actual buffer of scalar data stored in the class.
    // Interaction with the data (updating it on CPU or GPU side, accessing it, etc) happens through this wrapper.
    render::ManagedBuffer<uint32_t> values_buffer_;

    // === Get/set visualization parameters

    // The color map
    // QuantityT* setColorMap(std::string val);
    // std::string getColorMap();

protected:
    std::vector<uint32_t> values_data_;

    // === Visualization parameters

    // Affine data maps and limits
    size_t n_true_ = 0;
    size_t n_false_ = 0;

    // Parameters
    PersistentValue<bool> true_enabled_;
    PersistentValue<bool> false_enabled_;
    PersistentValue<glm::vec3> true_color_;
    PersistentValue<glm::vec3> false_color_;

    void count_values();
};

} // namespace polyscope

#include "BoolQuantity.ipp"
