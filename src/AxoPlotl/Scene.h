#pragma once

#include "AxoPlotl/geometry/nodes/ConvexPolygonNode.h"
#include "AxoPlotl/geometry/nodes/ExplicitCurveNode.h"
#include "AxoPlotl/geometry/nodes/ExplicitSurfaceNode.h"
#include "AxoPlotl/geometry/nodes/GeometryNode.h"
#include "AxoPlotl/geometry/Surface.h"
#include "AxoPlotl/geometry/nodes/ImplicitSurfaceNode.h"
#include "AxoPlotl/geometry/nodes/MeshNode.h"
#include "AxoPlotl/geometry/nodes/SphericalHarmonicNode.h"
#include "AxoPlotl/geometry/nodes/VectorFieldNode.h"
#include "AxoPlotl/rendering/redraw_frames.h"
#include "AxoPlotl/IO/FileAccessor.h"
#include "AxoPlotl/typedefs/typedefs_OpenVolumeMesh.hpp"
#include "rendering/PickingTexture.h"
#include "glad/glad.h"
#include "commons/Camera.h"
#include "rendering/MeshRenderer.h"
#include <nlohmann/json.hpp>

namespace AxoPlotl

{

class Scene
{
private:
    bool auto_zoom_to_new_object_ = true;

protected:
    std::vector<std::unique_ptr<GeometryNode>> objects_;

    GL::MeshRenderer gizmoRenderer_;
    CameraSet camera_set_;
    Vec3f clear_color_ = Vec3f(1,1,1);

    PickingTexture pickingTexture_;
    PickingTexture::Pixel picked_ = {0,0,0};

    void renderUI(GLFWwindow* window);

public:
    Scene()
    {}

    ~Scene() {

    }

    /// Call once before rendering for the first time
    virtual void init();

    void update(GLFWwindow* window);

    /// Render the scene and UI
    void render(GLFWwindow* window);

    template<typename Object, typename ...Args>
    inline void add_object(Args... _args) {
        objects_.push_back(std::make_unique<Object>(_args...));
        objects_.back()->init(this);
        if (auto_zoom_to_new_object_) {
            auto bbox = objects_.back()->getBBox();
            camera_set_.zoomToBox(bbox.first, bbox.second);
        }
        Rendering::triggerRedraw();
    }

    inline void add_mesh(const std::filesystem::path& _path) {
        auto mesh = IO::read_mesh(_path);
        if (mesh.has_value()) {
            if (std::holds_alternative<SurfaceMesh>(mesh.value())) {
                add_object<MeshNode>(volume_mesh(std::get<SurfaceMesh>(mesh.value())), _path.stem());
            } else if(std::holds_alternative<VolumeMesh>(mesh.value())) {
                add_object<MeshNode>(std::move(std::get<VolumeMesh>(mesh.value())), _path.stem());
            }
        }
    }

    bool saveToFile(const std::filesystem::path& filename);

    bool loadFromFile(const std::filesystem::path& filename);

    inline CameraSet& cameraSet() {
        return camera_set_;
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Scene,
    camera_set_, clear_color_)
};

class TestScene : public Scene
{
public:
    void init() override;
};

}
