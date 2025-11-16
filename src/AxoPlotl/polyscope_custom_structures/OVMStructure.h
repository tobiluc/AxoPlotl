#pragma once

#include "AxoPlotl/commons/ovm.h"
#include "polyscope/curve_network.h"
#include "polyscope/point_cloud.h"
#include "polyscope/structure.h"
#include "polyscope/surface_mesh.h"
#include "polyscope/volume_mesh.h"

#include <Eigen/Core>

// namespace OpenVolumeMesh
// {
// template <> OVM_EXPORT const std::string entityTypeName<Eigen::Matrix3d>();
// template <> OVM_EXPORT const std::string entityTypeName<Eigen::Vector3d>();
// }

namespace polyscope
{

class OVMStructure : public polyscope::QuantityStructure<OVMStructure>
{
public:
    // === Member functions ===

    // Construct a new structure
    OVMStructure(const std::string& _name, const AxoPlotl::PolyhedralMesh& _mesh);

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

    Structure* setEnabled(bool newEnabled) override;

    inline size_t n_vertices() const {
        return vertices_point_cloud_? vertices_point_cloud_->nPoints() : 0;
    }

    inline size_t n_edges() const {
        return edges_curve_network_? edges_curve_network_->nEdges() : 0;
    }

    inline size_t n_faces() const {
        return halffaces_surface_mesh_? (halffaces_surface_mesh_->nFaces()/2) : 0;
    }

    inline size_t n_cells() const {
        return cells_volume_mesh_? cells_volume_mesh_->nCells() : 0;
    }

    //------------------------
    // Quantities/Properties
    //------------------------

private:
    std::unique_ptr<PointCloud> vertices_point_cloud_;
    std::unique_ptr<CurveNetwork> edges_curve_network_;
    std::unique_ptr<SurfaceMesh> halffaces_surface_mesh_;
    std::unique_ptr<VolumeMesh> cells_volume_mesh_;

    float vertex_radius_ = 0.005f;
    float edge_radius_ = 0.005f;
    float cell_scale_ = 0.8f;
};

OVMStructure* registerOVMStructure(const std::string& name, const AxoPlotl::PolyhedralMesh& _mesh);

}
