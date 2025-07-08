#pragma once

#include "AxoPlotl/IO/OBJFileAccessor.h"
#include "AxoPlotl/IO/OVMFileAccessor.h"
#include "AxoPlotl/geometry/Surface.h"
#include "AxoPlotl/rendering/Renderer.h"
#include "AxoPlotl/IO/OVMFileAccessor.h"
#include "imgui.h"
#include <string>

namespace AxoPlotl
{

using Renderer = Rendering::Renderer;

class Scene;

class TriMesh
{
    std::vector<Vec3f> vertices;
    std::vector<Vec3i> triangles;
};

class AxPlGeometryObject
{
protected:
    int id_;
    static int id_counter_;
    std::string type_name_ = "AxPlObject";;
    char name_[64] = "BASE";
    Color ui_color_;
    bool deleted_ = false;
    bool show_ui_body_ = false;
    Rendering::Renderer renderer_;
    Rendering::Renderer::BatchIndices renderLoc_;
    glm::mat4 transform_; // model matrix

    /// General UI
    void renderUIHeader();

    /// Instance specific UI
    virtual void renderUIBody(Scene* scene) = 0;

public:
    AxPlGeometryObject(const std::string& _type_name, const std::string& _name, glm::mat4x4 _transform) :
        id_(++id_counter_), type_name_(_type_name), ui_color_(Color::random()),
        transform_(_transform)
    {
        std::strncpy(name_, _name.c_str(), sizeof(name_) - 1);
        name_[sizeof(name_) - 1] = '\0';
    }

    virtual ~AxPlGeometryObject() = default;

    /// Must be called before rendering
    virtual void addToRenderer(Scene* scene) = 0;

    inline void renderUI(Scene* scene) {
        ImGui::PushID(id_);
        renderUIHeader();
        if (show_ui_body_) {renderUIBody(scene);}
        ImGui::PopID();
    }

    inline void render(const glm::mat4& view, const glm::mat4& projection) {
        Rendering::Renderer::RenderMatrices matrices(transform_, view, projection);
        renderer_.render(matrices);
    }

    inline void renderPicking(const glm::mat4& view, const glm::mat4& projection) {
        Rendering::Renderer::RenderMatrices matrices(transform_, view, projection);
        renderer_.renderPicking(matrices.model_view_projection_matrix, id_);
    }

    inline bool isDeleted() const {return deleted_;}
};

// class EmptyObject : public AxPlGeometryObject
// {
// protected:
//     inline void addToRenderer() override {}

// public:
//     EmptyObject()  : AxPlGeometryObject("Empty", "NULL") {}

//     inline void updateRenderUI() override {}

//     inline void render() override {};
// };

class TetrahedralMeshObject : public AxPlGeometryObject
{
private:
    std::string filepath_;
    TetrahedralMesh mesh_;

public:
    TetrahedralMeshObject(const std::string& _filepath, const glm::mat4& _transform = glm::mat4(1.0)) :
        AxPlGeometryObject("Tet Mesh", std::filesystem::path(_filepath).filename(), _transform),
        filepath_(_filepath)
    {
        IO::readMesh(filepath_, mesh_);
    }

    void addToRenderer(Scene* scene) override;

    void renderUIBody(Scene* scene) override;
};

class HexahedralMeshObject : public AxPlGeometryObject
{
private:
    std::string filepath_;
    HexahedralMesh mesh_;

public:
    HexahedralMeshObject(const std::string& _filepath, const glm::mat4& _transform = glm::mat4(1.0)) :
        AxPlGeometryObject("Hex Mesh", std::filesystem::path(_filepath).filename(), _transform),
        filepath_(_filepath)
    {
        IO::readMesh(filepath_, mesh_);
    }

    HexahedralMeshObject(const HexahedralMesh& _mesh, const glm::mat4& _transform = glm::mat4(1.0)) :
        AxPlGeometryObject("Hex Mesh", std::filesystem::path("NONAME").filename(), _transform),
        filepath_(""), mesh_(_mesh)
    {;
    }

    void addToRenderer(Scene* scene) override;

    void renderUIBody(Scene* scene) override;
};

class MeshObject : public AxPlGeometryObject
{
private:
    std::string filepath_;
    Mesh mesh_;

public:
    MeshObject(const std::string& _filepath, const glm::mat4& _transform = glm::mat4(1.0)) :
        AxPlGeometryObject("Mesh", std::filesystem::path(_filepath).filename(), _transform),
        filepath_(_filepath)
    {;
        IO::loadMeshOBJ(_filepath, mesh_);
    }

    void addToRenderer(Scene* scene) override;

    void renderUIBody(Scene* scene) override;
};

class ExplicitSurfaceObject : public AxPlGeometryObject
{
private:
    ExplicitSurfaceFunction f_;
    ColorFunction2f color_;
    uint resolution_ = 32;
    char input_buffer_[1024];

public:
    ExplicitSurfaceObject(const std::string& _name, const ExplicitSurfaceFunction& _f,
                          Color _color = Color::BLUE, const glm::mat4& _transform = glm::mat4(1.0)) :
        AxPlGeometryObject("Explicit Surface", _name, _transform),  f_(_f), color_(_color)
    {}

    void addToRenderer(Scene* scene) override;

    void renderUIBody(Scene* scene) override;
};

class ImplicitSurfaceObject : public AxPlGeometryObject
{
private:
    ImplicitSurfaceFunction f_;
    Color color_;
    int resolution_ = 8;
    char input_buffer_[1024];

public:
    ImplicitSurfaceObject(const std::string& _name, const ImplicitSurfaceFunction& _f,
            Color _color = Color::RED, const glm::mat4& _transform = glm::mat4(1.0)) :
        AxPlGeometryObject("Implicit Surface", _name, _transform), f_(_f), color_(_color)
    {
        std::strncpy(input_buffer_, _f.str.c_str(), sizeof(input_buffer_) - 1);
        input_buffer_[sizeof(input_buffer_) - 1] = '\0';
    }

    void addToRenderer(Scene* scene) override;

    void renderUIBody(Scene* scene) override;
};

}
