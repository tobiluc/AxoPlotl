#pragma once

#include "AxoPlotl/geometry/GeometryObject.h"
#include "AxoPlotl/geometry/Surface.h"
#include "commons/PickingTexture.h"
#include "glad/glad.h"
#include "commons/Camera.h"
#include "rendering/MeshRenderer.h"
#include "rendering/Renderer.h"
#include <nlohmann/json.hpp>

namespace AxoPlotl

{

class Scene
{
protected:
    std::vector<std::unique_ptr<AxPlGeometryObject>> objects_; // TODO: When closing application, Crash: Error prob. cause of missing cleanup?

    Renderer gizmoRenderer_;
    Camera camera_;
    Color clearColor_ = Color::WHITE;

    PickingTexture pickingTexture_;
    PickingTexture::Pixel picked_ = {0,0,0};

    void renderUI();

public:
    Scene() :
        camera_(Vec3f(0,0,1), Vec3f(0,0,-1)),
        clearColor_(Color::WHITE)
    {}

    /// Call once before rendering for the first time
    virtual void init();

    /// Render the scene and UI
    void render(GLFWwindow* window);

    inline void addTetrahedralMesh(const std::string& filename) {
        objects_.push_back(std::make_unique<TetrahedralMeshObject>(filename));
        objects_.back()->addToRenderer();
    }

    inline void addExplicitSurface(const std::string& name, const ExplicitSurfaceFunction& func, Color color = Color::BLUE) {
        objects_.push_back(std::make_unique<ExplicitSurfaceObject>(name, func, color));
        objects_.back()->addToRenderer();
    }

    inline void addImplicitSurface(const std::string& name, const ImplicitSurfaceFunction& func, Color color = Color::RED) {
        objects_.push_back(std::make_unique<ImplicitSurfaceObject>(name, func, color));
        objects_.back()->addToRenderer();
    }

    void zoomToObject(int id);

    inline bool saveScene(const std::string& filename) {
        return IO::serialize(filename, *this);
    }

    inline bool loadScene(const std::string& filename) {
        return false; // TODO
        //return IO::deserialize(filename, *this);
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Scene,
    camera_)
};

class TestScene : public Scene
{
public:
    void init() override;
};

}
