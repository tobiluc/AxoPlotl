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
    static int id_counter_;
    char name_[64] = "BASE";
    Vec3f ui_color_;
    bool deleted_ = false;
    bool show_ui_body_ = false;
    //GL::Renderer renderer_;
    GL::MeshRenderer mesh_renderer_;
    //Rendering::Renderer::BatchIndices renderLoc_;
    glm::mat4 transform_; // model matrix
    std::pair<Vec3f,Vec3f> bbox_;

    /// General UI
    void renderUIHeader(Scene* scene);

    /// Instance specific UI
    virtual void renderUIBody(Scene* scene) = 0;

public:
    GeometryNode(const std::string& _name,
                 glm::mat4x4 _transform = glm::mat4x4(1)) :
        id_(++id_counter_), ui_color_(Vec3f(1,0,0)),
        transform_(_transform)
    {
        std::strncpy(name_, _name.c_str(), sizeof(name_) - 1);
        name_[sizeof(name_) - 1] = '\0';
    }

    virtual ~GeometryNode() = default;

    /// Must be called before rendering
    virtual void init(Scene* scene) = 0;

    inline std::pair<Vec3f, Vec3f> getBBox() const {
        // std::cerr << bbox_.first[0] << " "<<bbox_.first[1] << " "<<bbox_.first[2] <<std::endl;
        // std::cerr << bbox_.second[0] << " "<<bbox_.second[1] << " "<<bbox_.second[2] <<std::endl;
        // return bbox_;
        return {
            Vec3f(transform_ * Vec4f(bbox_.first, 1.0f)),
            Vec3f(transform_ * Vec4f(bbox_.second, 1.0f))
        };
    }

    inline void renderUI(Scene* scene) {
        ImGui::PushID(id_);
        renderUIHeader(scene);
        if (show_ui_body_) {
            renderUIBody(scene);
        }
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

    inline bool& visible() {return mesh_renderer_.visible;}
};

}
