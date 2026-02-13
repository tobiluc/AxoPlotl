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
        auto& r = scalar_range<Entity>(_r);
        if constexpr(std::is_same_v<ST,int>) {
            Vec2i i = {r.min_value,r.max_value};
            ImGui::SliderInt2("Range", &i.x, min, max);
            r.min_value = i.x;
            r.max_value = i.y;
        }
        else if constexpr(std::is_same_v<ST,float> || std::is_same_v<ST,double>) {
            Vec2f f = {r.min_value,r.max_value};
            ImGui::SliderFloat2("Range", &f.x, min, max);
            r.min_value = f.x;
            r.max_value = f.y;
        } else if constexpr(std::is_same_v<ST,bool>) {
            bool b_show_false = !r.min_value;
            bool b_show_true = r.max_value;
            ImGui::Checkbox("False", &b_show_false);
            ImGui::Checkbox("True", &b_show_true);
            r.min_value = !b_show_false;
            r.max_value = b_show_true;
        }

        ImGui::ColorEdit3("Min Color", &r.min_color[0]);
        ImGui::ColorEdit3("Max Color", &r.max_color[0]);
    }

    std::string name() override {
        return "Scalar Range";
    }

    ST min;
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
        ImGui::ColorEdit3("Color", &r.min_color[0]);
        r.max_color = r.min_color;
    }

    std::string name() override {
        return "Exact Scalar";
    }
};

}
