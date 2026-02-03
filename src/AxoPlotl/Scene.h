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
#include "rendering/PickingTexture.h"
#include "glad/glad.h"
#include "commons/Camera.h"
#include "rendering/MeshRenderer.h"
#include <nlohmann/json.hpp>

namespace AxoPlotl

{

class Scene
{
protected:
    std::vector<std::unique_ptr<GeometryNode>> objects_;

    GL::MeshRenderer gizmoRenderer_;
    CameraSet camera_set_;
    Color clearColor_ = Color::WHITE;

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

    inline void addMesh(const PolyhedralMesh& mesh, const std::string& name) {
        objects_.push_back(std::make_unique<MeshNode>(mesh, name));
        objects_.back()->initRenderer(this);
        Rendering::triggerRedraw();
    }

    inline void addVectorField(const std::function<Vec3f(Vec3f)>& func, const std::string& name) {
        objects_.push_back(std::make_unique<VectorFieldNode>(GradientField(func), name));
        objects_.back()->initRenderer(this);
        Rendering::triggerRedraw();
    }

    inline void addSphericalHarmonic(const std::string& name, const SphericalHarmonicFunction& sh) {
        objects_.push_back(std::make_unique<SphericalHarmonicNode>(sh, name));
        objects_.back()->initRenderer(this);
        Rendering::triggerRedraw();
    }

    inline void addConvexPolygon(const std::vector<Vec3f>& vertices, const std::string& name) {
        objects_.push_back(std::make_unique<ConvexPolygonNode>(vertices, name));
        objects_.back()->initRenderer(this);
        Rendering::triggerRedraw();
    }

    inline void addExplicitCurve(const std::string& name, const ExplicitCurveFunction& func, Color color = Color::BLUE) {
        objects_.push_back(std::make_unique<ExplicitCurveNode>(name, func, color));
        objects_.back()->initRenderer(this);
        Rendering::triggerRedraw();
    }

    inline void addExplicitSurface(const std::string& name, const ExplicitSurfaceFunction& func, Color color = Color::BLUE) {
        objects_.push_back(std::make_unique<ExplicitSurfaceNode>(name, func, color));
        objects_.back()->initRenderer(this);
        Rendering::triggerRedraw();
    }

    inline void addImplicitSurface(const std::string& name, const ImplicitSurfaceFunction& func, Color color = Color::RED) {
        objects_.push_back(std::make_unique<ImplicitSurfaceNode>(name, func, color));
        objects_.back()->initRenderer(this);
        Rendering::triggerRedraw();
    }

    inline bool saveToFile(const std::string& filename) {
        return IO::serialize(filename, *this);
    }

    inline bool loadFromFile(const std::string& filename) {
        // Scene scene;
        // if (IO::deserialize(filename, scene)) {
        //     this->clearColor_ = scene.clearColor_;
        //     // TODO
        //     return true;
        // } else {
        //     return false;
        // }
        return false;
    }

    inline CameraSet& cameraSet() {
        return camera_set_;
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Scene,
    camera_set_, clearColor_)
};

class TestScene : public Scene
{
public:
    void init() override;
};

}
