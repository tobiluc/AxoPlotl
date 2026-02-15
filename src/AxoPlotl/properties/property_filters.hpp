#pragma once

#include "AxoPlotl/rendering/VolumeMeshRenderer.hpp"
#include <AxoPlotl/rendering/MeshRenderer.h>
#include <type_traits>
#include <imgui.h>

namespace AxoPlotl
{

struct PropertyFilterBase
{
    virtual void renderUI(VolumeMeshRenderer& _r) = 0;
    virtual std::string name() = 0;
};

template<typename Entity>
static VolumeMeshRenderer::Property& get_property(VolumeMeshRenderer& _r)
{
    if constexpr(std::is_same_v<Entity,OVM::Entity::Vertex>) {return _r.vertex_property_;}
    if constexpr(std::is_same_v<Entity,OVM::Entity::Edge>) {return _r.edge_property_;}
    if constexpr(std::is_same_v<Entity,OVM::Entity::Face>) {return _r.face_property_;}
    if constexpr(std::is_same_v<Entity,OVM::Entity::Cell>) {return _r.cell_property_;}
}

template<typename ST, typename Entity>
struct ScalarPropertyRangeFilter : public PropertyFilterBase
{
    using Scalar = ST;

    ScalarPropertyRangeFilter(ST _min=0, ST _max=1)
        : min(_min), max(_max) {}

    void renderUI(VolumeMeshRenderer& _r) override
    {
        VolumeMeshRenderer::Property& rp = get_property<Entity>(_r);

        std::string colormap_menu_title = "Colormap ("
            + rp.color_map_.name_ + ")";
        if (ImGui::BeginMenu(colormap_menu_title.c_str())) {
            if (ImGui::MenuItem("Viridis")) {
                rp.color_map_.set_viridis(256);
            }
            if (ImGui::MenuItem("Magma")) {
                rp.color_map_.set_magma(256);
            }
            if (ImGui::MenuItem("Inferno")) {
                rp.color_map_.set_inferno(256);
            }
            if (ImGui::MenuItem("Plasma")) {
                rp.color_map_.set_plasma(256);
            }
            if (ImGui::MenuItem("Diverging Red Blue")) {
                rp.color_map_.set_rd_bu(256);
            }
            if (ImGui::MenuItem("Coolwarm")) {
                rp.color_map_.set_coolwarm(256);
            }
            ImGui::EndMenu();
        }

        auto draw_colormap = [&]() {
            ImGui::Image(
                (ImTextureID)(intptr_t)rp.color_map_.texture_id_,
                ImVec2(ImGui::GetContentRegionAvail().x, 20),
                ImVec2(0,0),
                ImVec2(1,1)
            );
            //ImGui::Spacing();
        };
        draw_colormap();

        // Slider
        Vec2f& vis_range = rp.range_;
        if constexpr(std::is_same_v<ST,int>) {
            Vec2i i = {vis_range[0],vis_range[1]};
            ImGui::SliderInt2("Show Range", &i.x, min, max);
            vis_range.x = i.x;
            vis_range.y = i.y;
        }
        else if constexpr(std::is_same_v<ST,float> || std::is_same_v<ST,double>) {
            Vec2f f = {vis_range.x,vis_range.y};
            ImGui::SliderFloat2("Show Range", &f.x, min, max);
            vis_range.x = std::clamp<float>(f.x, min, vis_range.y);
            vis_range.y = std::clamp<float>(f.y, vis_range.x, max);
        } else if constexpr(std::is_same_v<ST,bool>) {
            bool b_show_false = !vis_range.x;
            bool b_show_true = vis_range.y;
            ImGui::Checkbox("Show False", &b_show_false);
            ImGui::SameLine();
            ImGui::Checkbox("Show True", &b_show_true);
            vis_range.x = !b_show_false;
            vis_range.y = b_show_true;
        }
    }

    std::string name() override {
        return "Scalar Range";
    }

    ST min; // global min/max
    ST max;
};

template<typename ST, typename Entity>
struct ScalarPropertyExactFilter : public PropertyFilterBase
{
    using Scalar = ST;

    void renderUI(VolumeMeshRenderer& _r) override
    {
        VolumeMeshRenderer::Property& rp = get_property<Entity>(_r);
        Vec2f& r = rp.range_;

        //_r.v_prop_range
        if constexpr(std::is_same_v<ST,int>) {
            int i = r[0];
            ImGui::InputInt("Value", &i);
            r[0] = r[1] = i;
        }
        else if constexpr(std::is_same_v<ST,float> || std::is_same_v<ST,double>) {
            float f = r[0];
            ImGui::InputFloat("Value", &f);
            r[0] = r[1] = f;
        }
        else if constexpr(std::is_same_v<ST,bool>) {
            bool b = r[0];
            ImGui::Checkbox("True", &b);
            r[0] = r[1] = b;
        }

        if (ImGui::ColorEdit3("Color", &color[0])) {
            rp.color_map_.update({color[0],color[1],color[2],1.0f});
        }
    }

    std::string name() override {
        return "Exact Scalar";
    }

    Vec3f color = {1,0,0};
};

}
