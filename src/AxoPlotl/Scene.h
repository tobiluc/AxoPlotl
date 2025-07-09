#pragma once

#include "AxoPlotl/geometry/nodes/ConvexPolygonNode.h"
#include "AxoPlotl/geometry/nodes/ExplicitCurveNode.h"
#include "AxoPlotl/geometry/nodes/ExplicitSurfaceNode.h"
#include "AxoPlotl/geometry/nodes/GeometryNode.h"
#include "AxoPlotl/geometry/Surface.h"
#include "AxoPlotl/geometry/nodes/ImplicitSurfaceNode.h"
#include "AxoPlotl/geometry/nodes/MeshNode.h"
#include "AxoPlotl/geometry/nodes/VectorFieldNode.h"
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
    std::vector<std::unique_ptr<GeometryNode>> objects_; // TODO: When closing application, Crash: Error prob. cause of missing cleanup?

    Rendering::Renderer gizmoRenderer_;
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

    inline void addMesh(const Mesh& mesh, const std::string& name) {
        objects_.push_back(std::make_unique<MeshNode>(mesh, name));
        objects_.back()->addToRenderer(this);
    }

    // inline void addTetrahedralMesh(const std::string& filename) {
    //     objects_.push_back(std::make_unique<TetrahedralMeshObject>(filename));
    //     objects_.back()->addToRenderer(this);
    // }

    // inline void addHexahedralMesh(const std::string& filename) {
    //     objects_.push_back(std::make_unique<HexahedralMeshObject>(filename));
    //     objects_.back()->addToRenderer(this);
    // }

    // inline void addHexahedralMesh(const HexahedralMesh& mesh) {
    //     objects_.push_back(std::make_unique<HexahedralMeshObject>(mesh));
    //     objects_.back()->addToRenderer(this);
    // }

    inline void addVectorField(const std::function<float(Vec3f)>& func, const std::string& name) {
        objects_.push_back(std::make_unique<VectorFieldNode>(GradientField(func), name));
        objects_.back()->addToRenderer(this);
    }

    inline void addConvexPolygon(const std::vector<Vec3f>& vertices, const std::string& name) {
        objects_.push_back(std::make_unique<ConvexPolygonNode>(vertices, name));
        objects_.back()->addToRenderer(this);
    }

    inline void addExplicitCurve(const std::string& name, const ExplicitCurveFunction& func, Color color = Color::BLUE) {
        objects_.push_back(std::make_unique<ExplicitCurveNode>(name, func, color));
        objects_.back()->addToRenderer(this);
    }

    inline void addExplicitSurface(const std::string& name, const ExplicitSurfaceFunction& func, Color color = Color::BLUE) {
        objects_.push_back(std::make_unique<ExplicitSurfaceNode>(name, func, color));
        objects_.back()->addToRenderer(this);
    }

    inline void addImplicitSurface(const std::string& name, const ImplicitSurfaceFunction& func, Color color = Color::RED) {
        objects_.push_back(std::make_unique<ImplicitSurfaceNode>(name, func, color));
        objects_.back()->addToRenderer(this);
    }

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
