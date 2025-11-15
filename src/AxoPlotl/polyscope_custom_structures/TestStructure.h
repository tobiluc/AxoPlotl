#pragma once

#include "AxoPlotl/commons/ovm.h"
#include "polyscope/curve_network.h"
#include "polyscope/point_cloud.h"
#include "polyscope/structure.h"
#include "polyscope/surface_mesh.h"

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
        return faces_surface_mesh_? faces_surface_mesh_->nFaces() : 0;
    }

private:
    std::unique_ptr<PointCloud> vertices_point_cloud_;
    std::unique_ptr<CurveNetwork> edges_curve_network_;
    std::unique_ptr<SurfaceMesh> faces_surface_mesh_;

    float vertex_radius_;
    float edge_radius_;
};

OVMStructure* registerOVMStructure(const std::string& name, const AxoPlotl::PolyhedralMesh& _mesh);

}
