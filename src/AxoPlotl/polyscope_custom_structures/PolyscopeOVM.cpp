#include "PolyscopeOVM.h"

namespace polyscope
{

PolyscopeOVM::PolyscopeOVM(const std::string& _name)
:
    QuantityStructure<PolyscopeOVM>(_name, "OpenVolumeMesh"),

    vertex_positions_buffer_(this, uniquePrefix() + "vertexPositions", vertex_positions_data_),

    edge_from_inds_buffer_(this, uniquePrefix() + "edgeTailInds", edge_from_inds_data_),
    edge_to_inds_buffer_(this, uniquePrefix() + "edgeTipInds", edge_to_inds_data_),

    triangle_vertex_inds_buffer_(this, uniquePrefix() + "triangleVertexInds", triangle_vertex_inds_data_),
    triangle_halfface_inds_buffer_(this, uniquePrefix() + "triangleHalfFaceInds", triangle_halfface_inds_data_),
    halfface_normals_buffer_(this, uniquePrefix() + "halfFaceNormals", halfface_normals_data_),
    triangle_barycoord_buffer_(this, uniquePrefix() + "triangleBaryCoord", triangle_barycoord_data_),

    vertex_vis_cfg_(uniquePrefix()),
    edge_vis_cfg_(uniquePrefix()),
    face_vis_cfg_(uniquePrefix()),
    cell_vis_cfg_(uniquePrefix())
{

}

PolyscopeOVM::PolyscopeOVM(const std::string& _name, const OVM::PolyhedralMesh& _mesh)
:
    PolyscopeOVM(_name)
{
    // Fill Data
    vertex_positions_data_ = standardizeVectorArray<glm::vec3,3>(_mesh.vertex_positions());

    edge_from_inds_data_.reserve(_mesh.n_edges());
    edge_to_inds_data_.reserve(_mesh.n_edges());
    for (auto e_it = _mesh.e_iter(); e_it.is_valid(); ++e_it) {
        const auto& e = _mesh.edge(*e_it);
        edge_from_inds_data_.push_back(e.from_vertex().uidx());
        edge_to_inds_data_.push_back(e.to_vertex().uidx());
    }

    triangle_vertex_inds_data_.reserve(3*_mesh.n_halffaces());
    triangle_halfface_inds_data_.reserve(3*_mesh.n_halffaces());
    halfface_normals_data_.reserve(_mesh.n_halffaces());
    triangle_barycoord_data_.reserve(3*_mesh.n_halffaces());
    for (auto hf_it = _mesh.hf_iter(); hf_it.is_valid(); ++hf_it) {
        const auto& vhs = _mesh.get_halfface_vertices(*hf_it);
        for (uint32_t i = 1; i < vhs.size()-1; ++i) {

            // Triangle
            triangle_vertex_inds_data_.push_back(vhs[0].uidx());
            triangle_vertex_inds_data_.push_back(vhs[i].uidx());
            triangle_vertex_inds_data_.push_back(vhs[i+1].uidx());

            triangle_halfface_inds_data_.push_back(hf_it->uidx());
            triangle_halfface_inds_data_.push_back(hf_it->uidx());
            triangle_halfface_inds_data_.push_back(hf_it->uidx());

            triangle_barycoord_data_.emplace_back(1, 0, 0);
            triangle_barycoord_data_.emplace_back(0, 1, 0);
            triangle_barycoord_data_.emplace_back(0, 0, 1);
        }
        const auto& n = _mesh.normal(*hf_it);
        halfface_normals_data_.emplace_back(n[0], n[1], n[2]);
    }

    updateObjectSpaceBounds();
}

// === Overrides

// Build the imgui display
void PolyscopeOVM::buildCustomUI()
{
    ImGui::Text("#P = %zu, #E = %zu, #F = %zu, #C = %zu",
                n_vertices(), n_edges(), n_faces(), n_cells());

    if (ImGui::Button("Zoom to Object")) {
        glm::vec3 p = polyscope::view::getCameraWorldPosition();
        glm::vec3 o = 0.5f*(std::get<0>(objectSpaceBoundingBox) + std::get<1>(objectSpaceBoundingBox));
        if (p != o) {
            p = o + objectSpaceLengthScale * glm::normalize(p - o);
            polyscope::view::lookAt(p, o, true);
        }
    }
}

void PolyscopeOVM::buildCustomOptionsUI()
{
    if (n_vertices() && ImGui::CollapsingHeader("Vertex Render Options")) {
        ImGui::PushID("vertices");
        if (ImGui::SliderFloat("Radius", vertex_vis_cfg_.radius_.get().getValuePtr(), 0.0, .1, "%.5f",
                ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_NoRoundToFormat)) {
            vertex_vis_cfg_.radius_.manuallyChanged();
            requestRedraw();
        }
        ImGui::PopID();
    }
    if (n_edges() && ImGui::CollapsingHeader("Edge Render Options")) {
        ImGui::PushID("edges");
        if (ImGui::SliderFloat("Radius", edge_vis_cfg_.radius_.get().getValuePtr(), 0.0, .1, "%.5f",
                               ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_NoRoundToFormat)) {
            edge_vis_cfg_.radius_.manuallyChanged();
            requestRedraw();
        }
        ImGui::PopID();
    }
}

void PolyscopeOVM::buildPickUI(const PickResult& result) {}

void PolyscopeOVM::buildQuantitiesUI() {}

void PolyscopeOVM::buildStructureOptionsUI() {}

void PolyscopeOVM::draw()
{
    if (!isEnabled()) {return;}

    // If there is no dominant quantity, then this class is responsible for drawing
    if (dominantQuantity == nullptr) {

        // Draw Vertices
        check_vertex_render_program();
        if (vertex_render_program_)
        {
            setStructureUniforms(*vertex_render_program_);
            set_vertex_uniforms(*vertex_render_program_);
            render::engine->setMaterialUniforms(*vertex_render_program_, vertex_vis_cfg_.material_.get());
            vertex_render_program_->setUniform("u_baseColor", vertex_vis_cfg_.color_.get());

            vertex_render_program_->draw();
        }

        // Draw Edges
        check_edge_render_program();
        if (edge_render_program_)
        {
            setStructureUniforms(*edge_render_program_);
            set_edge_uniforms(*edge_render_program_);

            edge_render_program_->setUniform("u_baseColor", edge_vis_cfg_.color_.get());

            render::engine->setMaterialUniforms(*edge_render_program_, edge_vis_cfg_.material_.get());

            edge_render_program_->draw();
        }

        // Draw Faces
        check_face_render_program();
        if (face_render_program_)
        {
            render::engine->setBackfaceCull(true);

            setStructureUniforms(*face_render_program_);
            set_face_uniforms(*face_render_program_);
            render::engine->setMaterialUniforms(*face_render_program_, face_vis_cfg_.material_.get());
            face_render_program_->setUniform("u_baseColor", face_vis_cfg_.surface_color_.get());

            face_render_program_->draw();

            render::engine->setBackfaceCull(); // back to default
        }
    }

    // Draw the quantities
    for (auto& x : quantities) {
        x.second->draw();
    }
    for (auto& x : floatingQuantities) {
        x.second->draw();
    }
}

void PolyscopeOVM::drawDelayed()
{
    if (!isEnabled()) {return;}

    render::engine->setBackfaceCull(true);

    for (auto& x : quantities) {
        x.second->drawDelayed();
    }

    render::engine->setBackfaceCull(); // return to default setting

    for (auto& x : floatingQuantities) {
        x.second->drawDelayed();
    }
}

void PolyscopeOVM::drawPick()
{
    if (!isEnabled()) {return;}

    // Ensure we have prepared buffers
    check_vertex_pick_program();
    if (vertex_pick_program_)
    {
        setStructureUniforms(*vertex_pick_program_);
        set_vertex_uniforms(*vertex_pick_program_);

        vertex_pick_program_->draw();
    }

    for (auto& x : quantities) {
        x.second->drawPick();
    }
    for (auto& x : floatingQuantities) {
        x.second->drawPick();
    }
}

void PolyscopeOVM::drawPickDelayed() {}

void PolyscopeOVM::updateObjectSpaceBounds()
{
    vertex_positions_buffer_.ensureHostBufferPopulated();

    // bounding box
    glm::vec3 min = glm::vec3{1, 1, 1} * std::numeric_limits<float>::infinity();
    glm::vec3 max = -glm::vec3{1, 1, 1} * std::numeric_limits<float>::infinity();
    for (const glm::vec3& p : vertex_positions_buffer_.data) {
        min = componentwiseMin(min, p);
        max = componentwiseMax(max, p);
    }
    objectSpaceBoundingBox = std::make_tuple(min, max);

    // length scale, as twice the radius from the center of the bounding box
    glm::vec3 center = 0.5f * (min + max);
    float lengthScale = 0.0;
    for (const glm::vec3& p : vertex_positions_buffer_.data) {
        lengthScale = std::max(lengthScale, glm::length2(p - center));
    }
    objectSpaceLengthScale = 2 * std::sqrt(lengthScale);
}

std::string PolyscopeOVM::typeName() {
    return "OpenVolumeMesh";
}

void PolyscopeOVM::refresh() {
    // TODO
}

void PolyscopeOVM::check_vertex_render_program()
{
    if (vertex_render_program_) {return;}

    vertex_render_program_ = render::engine->requestShader(
        vertex_vis_cfg_.shader_name(),
        render::engine->addMaterialRules(vertex_vis_cfg_.material_.get(),
        add_vertex_render_rules({"SHADE_BASECOLOR"})));

    populate_vertex_geometry_buffers(*vertex_render_program_);

    render::engine->setMaterial(*vertex_render_program_, vertex_vis_cfg_.material_.get());
}

void PolyscopeOVM::check_vertex_pick_program()
{
    check_vertex_render_program();

    // Request pick indices
    size_t pickCount = n_vertices();
    size_t pickStart = pick::requestPickBufferRange(this, pickCount);

    // Create a new pick program
    vertex_pick_program_ = render::engine->requestShader(
        vertex_vis_cfg_.shader_name(),
        add_vertex_render_rules({"SPHERE_PROPAGATE_COLOR"}),
        render::ShaderReplacementDefaults::Pick
    );

    populate_vertex_geometry_buffers(*vertex_pick_program_);

    // Fill color buffer with packed point indices
    std::vector<glm::vec3> pickColors;
    for (size_t i = pickStart; i < pickStart + pickCount; i++) {
        glm::vec3 val = pick::indToVec(i);
        pickColors.push_back(pick::indToVec(i));
    }

    // Store data in buffers
    vertex_pick_program_->setAttribute("a_color", pickColors);
}

void PolyscopeOVM::check_edge_render_program()
{
    if (edge_render_program_) {return;}

    // clang-format off
    edge_render_program_ = render::engine->requestShader(
        "RAYCAST_CYLINDER",
        render::engine->addMaterialRules(
            edge_vis_cfg_.material_.get(),
            add_edge_render_rules({"SHADE_BASECOLOR"})));

    render::engine->setMaterial(*edge_render_program_, edge_vis_cfg_.material_.get());

    // Fill out the geometry data for the programs
    populate_edge_geometry_buffers(*edge_render_program_);
}

void PolyscopeOVM::check_edge_pick_program()
{
    check_edge_render_program();
}

void PolyscopeOVM::check_face_render_program()
{
    if (face_render_program_) {return;}

    face_render_program_ = render::engine->requestShader( "MESH",
      render::engine->addMaterialRules(face_vis_cfg_.material_.get(),
        add_face_render_rules({"SHADE_BASECOLOR"}, true)));

    // Populate draw buffers
    populate_face_geometry_buffers(*face_render_program_);
    render::engine->setMaterial(*face_render_program_, face_vis_cfg_.material_.get());
}

void PolyscopeOVM::check_face_pick_program()
{
    check_face_render_program();
}

void PolyscopeOVM::check_cell_render_program()
{

}

void PolyscopeOVM::check_cell_pick_program()
{
    check_cell_render_program();
}

std::vector<std::string> PolyscopeOVM::add_vertex_render_rules(std::vector<std::string> _init_rules)
{
    _init_rules = addStructureRules(_init_rules);
    return _init_rules;
}

std::vector<std::string> PolyscopeOVM::add_edge_render_rules(std::vector<std::string> _init_rules)
{
    _init_rules = addStructureRules(_init_rules);
    return _init_rules;
}

std::vector<std::string> PolyscopeOVM::add_face_render_rules(std::vector<std::string> _init_rules, bool _with_shade)
{
    _init_rules = addStructureRules(_init_rules);
    return _init_rules;
}

void PolyscopeOVM::set_vertex_uniforms(render::ShaderProgram& _p)
{
    glm::mat4 P = view::getCameraPerspectiveMatrix();
    glm::mat4 Pinv = glm::inverse(P);

    if (vertex_vis_cfg_.render_mode_.get() == "sphere") {
        _p.setUniform("u_invProjMatrix", glm::value_ptr(Pinv));
        _p.setUniform("u_viewport", render::engine->getCurrentViewport());
    }

    _p.setUniform("u_pointRadius", vertex_vis_cfg_.radius_.get().asAbsolute());
}

void PolyscopeOVM::set_edge_uniforms(render::ShaderProgram& _p)
{
  glm::mat4 P = view::getCameraPerspectiveMatrix();
  glm::mat4 Pinv = glm::inverse(P);
  _p.setUniform("u_invProjMatrix", glm::value_ptr(Pinv));
  _p.setUniform("u_viewport", render::engine->getCurrentViewport());
  _p.setUniform("u_radius", edge_vis_cfg_.radius_.get().asAbsolute());
}

void PolyscopeOVM::set_face_uniforms(render::ShaderProgram& _p)
{
  if (face_vis_cfg_.edge_width_.get() > 0) {
    _p.setUniform("u_edgeWidth", face_vis_cfg_.edge_width_.get() * render::engine->getCurrentPixelScaling());
    _p.setUniform("u_edgeColor", face_vis_cfg_.edge_color_.get());
  }
  if (face_vis_cfg_.shade_style_.get() == MeshShadeStyle::TriFlat) {
    glm::mat4 P = view::getCameraPerspectiveMatrix();
    glm::mat4 Pinv = glm::inverse(P);
    _p.setUniform("u_invProjMatrix", glm::value_ptr(Pinv));
    _p.setUniform("u_viewport", render::engine->getCurrentViewport());
  }
}

void PolyscopeOVM::set_cell_uniforms(render::ShaderProgram& _p)
{}

void PolyscopeOVM::populate_vertex_geometry_buffers(render::ShaderProgram& _p)
{
    _p.setAttribute("a_position", vertex_positions_buffer_.getRenderAttributeBuffer());
}

void PolyscopeOVM::populate_edge_geometry_buffers(render::ShaderProgram& _p)
{
  _p.setAttribute("a_position_tail", vertex_positions_buffer_.getIndexedRenderAttributeBuffer(edge_from_inds_buffer_));
  _p.setAttribute("a_position_tip", vertex_positions_buffer_.getIndexedRenderAttributeBuffer(edge_to_inds_buffer_));
}

void PolyscopeOVM::populate_face_geometry_buffers(render::ShaderProgram& _p)
{
  if (_p.hasAttribute("a_vertexPositions")) {
    _p.setAttribute("a_vertexPositions", vertex_positions_buffer_.getIndexedRenderAttributeBuffer(triangle_vertex_inds_buffer_));
  }
  if (_p.hasAttribute("a_vertexNormals")) {

    // TODO: Set normals correctly when shade style is smooth
    _p.setAttribute("a_vertexNormals", halfface_normals_buffer_.getIndexedRenderAttributeBuffer(triangle_halfface_inds_buffer_));
  }
  // if (p.hasAttribute("a_normal")) {
  //   p.setAttribute("a_normal", faceNormals.getIndexedRenderAttributeBuffer(triangleFaceInds));
  // }
  if (_p.hasAttribute("a_barycoord")) {
    _p.setAttribute("a_barycoord", triangle_barycoord_buffer_.getRenderAttributeBuffer());
  }
  // if (p.hasAttribute("a_edgeIsReal")) {
  //   p.setAttribute("a_edgeIsReal", edgeIsReal.getRenderAttributeBuffer());
  // }
  // if (wantsCullPosition()) {
  //   p.setAttribute("a_cullPos", faceCenters.getIndexedRenderAttributeBuffer(triangleFaceInds));
  // }

  // if (transparencyQuantityName != "") {
  //   SurfaceScalarQuantity& transparencyQ = resolveTransparencyQuantity();
  //   p.setAttribute("a_valueAlpha", transparencyQ.getAttributeBuffer());
  // }
}

void PolyscopeOVM::populate_cell_geometry_buffers(render::ShaderProgram& _p)
{}

PolyscopeOVM* register_polyscope_ovm(const std::string& _name, const OVM::PolyhedralMesh& _mesh)
{
    checkInitialized();
    auto* s = new PolyscopeOVM(_name, _mesh);
    bool success = polyscope::registerStructure(s);
    if (!success) {
        safeDelete(s);
        std::cerr << "Failed to register structure " << std::endl;
    }
    return s;
}

}
