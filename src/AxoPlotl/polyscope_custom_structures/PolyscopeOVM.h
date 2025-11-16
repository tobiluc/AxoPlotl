#pragma once
#include <OpenVolumeMesh/Core/GeometryKernel.hh>
#include <polyscope/quantity.h>

namespace OpenVolumeMesh
{
using PolyhedralMesh = GeometryKernel<Vec3d, TopologyKernel>;
}

namespace polyscope
{
namespace OVM = OpenVolumeMesh;

class PolyscopeOVM : public polyscope::QuantityStructure<PolyscopeOVM>
{
public:
    PolyscopeOVM(const std::string& _name);

    PolyscopeOVM(const std::string& _name, const OVM::PolyhedralMesh& _mesh);

    inline size_t n_vertices() const {
        return vertex_positions_buffer_.data.size();
    }

    inline size_t n_edges() const {
        return edge_from_inds_buffer_.data.size();
    }

    inline size_t n_faces() const {
        return halfface_normals_buffer_.data.size()/2;
    }

    inline size_t n_cells() const {
        return 0;
    }

    // === Overrides

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

private:
    //==============================
    // Buffers
    //==============================
    render::ManagedBuffer<glm::vec3> vertex_positions_buffer_;

    render::ManagedBuffer<uint32_t> edge_from_inds_buffer_; // E indices into the node list
    render::ManagedBuffer<uint32_t> edge_to_inds_buffer_;

    render::ManagedBuffer<uint32_t> triangle_vertex_inds_buffer_; // 3 * #Triangles
    render::ManagedBuffer<uint32_t> triangle_halfface_inds_buffer_; // 3 * #Triangles
    render::ManagedBuffer<glm::vec3> halfface_normals_buffer_; // #Halffaces
    render::ManagedBuffer<glm::vec3> triangle_barycoord_buffer_; // 3 * #Triangles

    //==============================
    // Data for Buffers above
    //==============================
    std::vector<glm::vec3> vertex_positions_data_;

    std::vector<uint32_t> edge_from_inds_data_;
    std::vector<uint32_t> edge_to_inds_data_;

    std::vector<uint32_t> triangle_vertex_inds_data_;
    std::vector<uint32_t> triangle_halfface_inds_data_;
    std::vector<glm::vec3> halfface_normals_data_; // [halffaces]
    std::vector<glm::vec3> triangle_barycoord_data_;

    //==============================
    // Visualization Settings
    //==============================
    struct VertexVisCfg {
        VertexVisCfg(const std::string& _unique_prefix) :
            render_mode_(_unique_prefix + "pointRenderMode", "sphere"),
            color_(_unique_prefix + "pointColor", getNextUniqueColor()),
            radius_(_unique_prefix + "pointRadius", relativeValue(0.002)),
            material_(_unique_prefix + "material", "clay")
        {}
        PersistentValue<std::string> render_mode_;
        PersistentValue<glm::vec3> color_;
        PersistentValue<ScaledValue<float>> radius_;
        PersistentValue<std::string> material_;

        inline std::string shader_name() {
            if (render_mode_.get() == "sphere") {
                return "RAYCAST_SPHERE";
            }
            if (render_mode_.get() == "quad") {
                return "POINT_QUAD";
            }
            return "ERROR";
        }
    } vertex_vis_cfg_;

    struct EdgeVisCfg {
        EdgeVisCfg(const std::string& _unique_prefix) :
            color_(_unique_prefix + "#color", getNextUniqueColor()),
            radius_(_unique_prefix + "#radius", relativeValue(0.001)),
            material_(_unique_prefix + "#material", "clay")
        {}
        PersistentValue<glm::vec3> color_;
        PersistentValue<ScaledValue<float>> radius_;
        PersistentValue<std::string> material_;
    } edge_vis_cfg_;

    struct FaceVisCfg {
        FaceVisCfg(const std::string& _unique_prefix) :
            surface_color_(_unique_prefix + "surfaceColor", getNextUniqueColor()),
            edge_color_(_unique_prefix + "edgeColor", glm::vec3{0., 0., 0.}), material_(_unique_prefix + "material", "clay"),
            edge_width_(_unique_prefix + "edgeWidth",       0.),
            shade_style_(_unique_prefix + "shadeStyle",      MeshShadeStyle::Flat),
            selection_mode_(_unique_prefix + "selectionMode",   MeshSelectionMode::Auto)
        {}

        PersistentValue<glm::vec3> surface_color_;
        PersistentValue<glm::vec3> edge_color_;
        PersistentValue<std::string> material_;
        PersistentValue<float> edge_width_;
        PersistentValue<MeshShadeStyle> shade_style_;
        PersistentValue<MeshSelectionMode> selection_mode_;
    } face_vis_cfg_;

    struct CellVisCfg {
        CellVisCfg(const std::string& _unique_prefix) :
            color_(_unique_prefix + "color", getNextUniqueColor()),
            interior_color_(_unique_prefix + "interiorColor", color_.get()),
            edge_color_(_unique_prefix + "edgeColor", glm::vec3{0., 0., 0.}),
            material_(_unique_prefix + "material", "clay"),
            edge_width_(_unique_prefix + "edgeWidth", 0.),
            cell_scale_(_unique_prefix + "cellScale", 1.0)
        {}
        PersistentValue<glm::vec3> color_;
        PersistentValue<glm::vec3> interior_color_;
        PersistentValue<glm::vec3> edge_color_;
        PersistentValue<std::string> material_;
        PersistentValue<float> edge_width_;
        PersistentValue<float> cell_scale_;
    } cell_vis_cfg_;

    std::shared_ptr<render::ShaderProgram> vertex_render_program_;
    std::shared_ptr<render::ShaderProgram> vertex_pick_program_;
    std::shared_ptr<render::ShaderProgram> edge_render_program_;
    std::shared_ptr<render::ShaderProgram> edge_pick_program_;
    std::shared_ptr<render::ShaderProgram> face_render_program_;
    std::shared_ptr<render::ShaderProgram> face_pick_program_;
    std::shared_ptr<render::ShaderProgram> cell_render_program_;
    std::shared_ptr<render::ShaderProgram> cell_pick_program_;

    void check_vertex_render_program();
    void check_vertex_pick_program();
    void check_edge_render_program();
    void check_edge_pick_program();
    void check_face_render_program();
    void check_face_pick_program();
    void check_cell_render_program();
    void check_cell_pick_program();

    std::vector<std::string> add_vertex_render_rules(std::vector<std::string> _init_rules);
    std::vector<std::string> add_edge_render_rules(std::vector<std::string> _init_rules);
    std::vector<std::string> add_face_render_rules(std::vector<std::string> _init_rules, bool _with_shade);

    void set_vertex_uniforms(render::ShaderProgram& _p);
    void set_edge_uniforms(render::ShaderProgram& _p);
    void set_face_uniforms(render::ShaderProgram& _p);
    void set_cell_uniforms(render::ShaderProgram& _p);

    void populate_vertex_geometry_buffers(render::ShaderProgram& _p);
    void populate_edge_geometry_buffers(render::ShaderProgram& _p);
    void populate_face_geometry_buffers(render::ShaderProgram& _p);
    void populate_cell_geometry_buffers(render::ShaderProgram& _p);
};

PolyscopeOVM* register_polyscope_ovm(const std::string& _name, const OVM::PolyhedralMesh& _mesh);

}
