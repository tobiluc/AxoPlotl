#pragma once

#include <AxoPlotl/rendering/MeshRenderer.h>
#include <type_traits>
#include <imgui.h>

namespace AxoPlotl
{

struct PropertyFilterBase
{
    virtual void renderUI(GL::MeshRenderer& _r) = 0;
    virtual std::string name() = 0;
};

template<typename Entity>
static GL::MeshRenderer::ScalarRangeConfig& scalar_range(GL::MeshRenderer& _r)
{
    if constexpr(std::is_same_v<Entity,OVM::Entity::Vertex>) {return _r.vertex_scalar_prop_range_;}
    if constexpr(std::is_same_v<Entity,OVM::Entity::Edge>) {return _r.edge_scalar_prop_range_;}
    if constexpr(std::is_same_v<Entity,OVM::Entity::Face>) {return _r.face_scalar_prop_range_;}
    if constexpr(std::is_same_v<Entity,OVM::Entity::Cell>) {return _r.cell_scalar_prop_range_;}
}

template<typename ST, typename Entity>
struct ScalarPropertyRangeFilter : public PropertyFilterBase
{
    using Scalar = ST;

    ScalarPropertyRangeFilter(ST _min=0, ST _max=1)
        : min(_min), max(_max) {}

    void renderUI(GL::MeshRenderer& _r) override
    {
        std::string colormap_menu_title = "Colormap ("
            + _r.color_map_.name_ + ")";
        if (ImGui::BeginMenu(colormap_menu_title.c_str())) {
            if (ImGui::MenuItem("Viridis")) {
                _r.color_map_.set_viridis(256);
            }
            if (ImGui::MenuItem("Magma")) {
                _r.color_map_.set_magma(256);
            }
            if (ImGui::MenuItem("Inferno")) {
                _r.color_map_.set_inferno(256);
            }
            if (ImGui::MenuItem("Plasma")) {
                _r.color_map_.set_plasma(256);
            }
            if (ImGui::MenuItem("Diverging Red Blue")) {
                _r.color_map_.set_rd_bu(256);
            }
            if (ImGui::MenuItem("Coolwarm")) {
                _r.color_map_.set_coolwarm(256);
            }
            ImGui::EndMenu();
        }

        auto draw_colormap = [&]() {
            ImGui::Image(
                (ImTextureID)(intptr_t)_r.color_map_.texture_id_,
                ImVec2(ImGui::GetContentRegionAvail().x, 20),
                ImVec2(0,0),
                ImVec2(1,1)
            );
            //ImGui::Spacing();
        };
        draw_colormap();

        // Slider
        auto& r = scalar_range<Entity>(_r);
        if constexpr(std::is_same_v<ST,int>) {
            Vec2i i = {r.min_value,r.max_value};
            ImGui::SliderInt2("Show Range", &i.x, min, max);
            r.min_value = i.x;
            r.max_value = i.y;
        }
        else if constexpr(std::is_same_v<ST,float> || std::is_same_v<ST,double>) {
            Vec2f f = {r.min_value,r.max_value};
            ImGui::SliderFloat2("Show Range", &f.x, min, max);
            r.min_value = std::clamp<float>(f.x, min, r.max_value);
            r.max_value = std::clamp<float>(f.y, r.min_value, max);
        } else if constexpr(std::is_same_v<ST,bool>) {
            bool b_show_false = !r.min_value;
            bool b_show_true = r.max_value;
            ImGui::Checkbox("Show False", &b_show_false);
            ImGui::SameLine();
            ImGui::Checkbox("Show True", &b_show_true);
            r.min_value = !b_show_false;
            r.max_value = b_show_true;
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

    void renderUI(GL::MeshRenderer& _r) override
    {
        auto& r = scalar_range<Entity>(_r);
        //_r.v_prop_range
        if constexpr(std::is_same_v<ST,int>) {
            int i = r.min_value;
            ImGui::InputInt("Value", &i);
            r.min_value = i;
            r.max_value = i;
        }
        else if constexpr(std::is_same_v<ST,float> || std::is_same_v<ST,double>) {
            float f = r.min_value;
            ImGui::InputFloat("Value", &f);
            r.min_value = f;
            r.max_value = f;
        }
        else if constexpr(std::is_same_v<ST,bool>) {
            bool b = r.min_value;
            ImGui::Checkbox("True", &b);
            r.min_value = b;
            r.max_value = b;
        }

        if (ImGui::ColorEdit3("Color", &color[0])) {
            _r.color_map_.update({color[0],color[1],color[2],1.0f});
        }
    }

    std::string name() override {
        return "Exact Scalar";
    }

    Vec3f color = {1,0,0};
};

}
