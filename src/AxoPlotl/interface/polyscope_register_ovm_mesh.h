#pragma once

#include "AxoPlotl/commons/ovm.h"
#include "polyscope/point_cloud.h"
#include "polyscope/surface_mesh.h"
#include <polyscope/volume_mesh.h>

namespace AxoPlotl::Interface
{

polyscope::PointCloud* register_point_cloud(const std::string& _name, const PolyhedralMesh& _mesh);

polyscope::CurveNetwork* register_curve_network(const std::string& _name, const PolyhedralMesh& _mesh);

polyscope::SurfaceMesh* register_surface_mesh(const std::string& _name, const PolyhedralMesh& _mesh);

polyscope::VolumeMesh* register_volume_mesh(const std::string& _name, const PolyhedralMesh& _mesh);

}
