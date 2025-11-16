namespace polyscope {

template <typename QuantityT>
BooleanQuantity<QuantityT>::BooleanQuantity(
    QuantityT& _quantity,
    const std::vector<uint32_t>& _values_data)
    :
    quantity(_quantity),
    values_buffer_(&quantity, quantity.uniquePrefix() + "values", values_data_),
    values_data_(_values_data),

    true_enabled_(quantity.uniquePrefix() + "trueEnabled", true),
    false_enabled_(quantity.uniquePrefix() + "falseEnabled", true),
    true_color_(quantity.uniquePrefix() + "trueColor", glm::vec3(0,1,0)),
    false_color_(quantity.uniquePrefix() + "falseColor", glm::vec3(1,0,0))
{
    count_values();
}

template <typename QuantityT>
void BooleanQuantity<QuantityT>::buildBooleanUI() {

    ImGui::Text("Boolean UI");
    ImGui::Text("True: %zu, False: %zu", n_true_, n_false_);
}

template <typename QuantityT>
void BooleanQuantity<QuantityT>::buildBooleanOptionsUI()
{
    ImGui::Text("Boolean Options UI");
}

template <typename QuantityT>
void BooleanQuantity<QuantityT>::setBooleanUniforms(render::ShaderProgram& p)
{
    p.setUniform("u_trueColor", true_color_.get());
    p.setUniform("u_falseColor", true_color_.get());
}

template <typename QuantityT>
template <class V>
void BooleanQuantity<QuantityT>::updateData(const V& newValues) {
    validateSize(newValues, values_buffer_.size(), "boolean quantity " + quantity.name);
    values_buffer_.data = standardizeArray<bool,V>(newValues);
    values_buffer_.markHostBufferUpdated();
    count_values();
}

template <typename QuantityT>
void BooleanQuantity<QuantityT>::count_values()
{
    n_true_ = std::count(values_data_.begin(), values_data_.end(), true);
    n_false_ = values_data_.size() - n_true_;
}

} // namespace polyscope
