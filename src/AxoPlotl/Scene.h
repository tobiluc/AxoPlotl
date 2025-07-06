#pragma once

#include "AxoPlotl/geometry/AxPlGeometryObject.h"
#include "AxoPlotl/geometry/presets.h"
#include "commons/PickingTexture.h"
#include "glad/glad.h"
#include "commons/Camera.h"
#include "rendering/MeshRenderer.h"
#include "rendering/Renderer.h"

namespace AxoPlotl

{

class Scene
{
protected:
    std::vector<std::unique_ptr<AxPlGeometryObject>> objects_; // TODO: When closing application, Crash: Error prob. cause of missing cleanup?

    Renderer renderer_;
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
    }

    inline void addExplicitSurface(const std::string& name, const ExplicitSurfaceFunction& func, Color color = Color::BLUE) {
        objects_.push_back(std::make_unique<ExplicitSurfaceObject>(name, func, color));
    }

    // inline void addImplicitSurface(const std::function<float(float,float,float)>& func, Color color = Color::RED) {
    //     objects_.push_back(std::make_unique<ImplicitSurfaceObject>(func, color));
    // }
};

class TestScene : public Scene
{
public:
    void init() override;
};

}
