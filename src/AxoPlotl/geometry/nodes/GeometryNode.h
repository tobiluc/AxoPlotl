#pragma once

#include "AxoPlotl/rendering/MeshRenderer.h"
#include <imgui.h>

namespace AxoPlotl
{

// Forward declarations
class Scene;

class GeometryNode
{
protected:
    int id_;
    bool target_ = false;
    static int id_counter_;
    std::string type_name_ = "AxPlObject";
    char name_[64] = "BASE";
    Vec3f ui_color_;
    bool deleted_ = false;
    bool show_ui_body_ = false;
    //GL::Renderer renderer_;
    GL::MeshRenderer mesh_renderer_;
    //Rendering::Renderer::BatchIndices renderLoc_;
    glm::mat4 transform_; // model matrix

    /// General UI
    void renderUIHeader(Scene* scene);

    /// Instance specific UI
    virtual void renderUIBody(Scene* scene) = 0;

public:
    GeometryNode(const std::string& _type_name,
                 const std::string& _name,
                 glm::mat4x4 _transform = glm::mat4x4(1)) :
        id_(++id_counter_), type_name_(_type_name), ui_color_(Vec3f(1,0,0)),
        transform_(_transform)
    {
        std::strncpy(name_, _name.c_str(), sizeof(name_) - 1);
        name_[sizeof(name_) - 1] = '\0';
    }

    virtual ~GeometryNode() = default;

    /// Must be called before rendering
    virtual void init(Scene* scene) = 0;

    virtual std::pair<glm::vec3, glm::vec3> getBBox() = 0;

    inline void renderUI(Scene* scene) {
        ImGui::PushID(id_);
        renderUIHeader(scene);
        if (show_ui_body_) {renderUIBody(scene);}
        ImGui::PopID();
    }

    inline void render(const glm::mat4& view, const glm::mat4& projection) {
        GL::MeshRenderer::Matrices m(transform_, view, projection);
        mesh_renderer_.render(m);
    }

    inline void renderPicking(const glm::mat4& view, const glm::mat4& projection) {
        GL::MeshRenderer::Matrices m(transform_, view, projection);
        mesh_renderer_.renderPicking(m, id_);
    }

    inline int id() const {return id_;}

    inline const char* name() const {return name_;}

    inline bool isDeleted() const {return deleted_;}

    inline void setDeleted() {deleted_ = true;}
};

}
