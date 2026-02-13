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

template<typename ST>
struct ScalarPropertyRangeFilter : public PropertyFilterBase
{
    using Scalar = ST;

    ScalarPropertyRangeFilter(ST _min=0, ST _max=1)
        : min(_min), max(_max) {}
    void renderUI(GL::MeshRenderer& _r) override
    {
        if constexpr(std::is_same_v<ST,int>) {
            Vec2i i = {_r.settings().scalar_property_range.min_value,_r.settings().scalar_property_range.max_value};
            ImGui::SliderInt2("Range", &i.x, min, max);
            _r.settings().scalar_property_range.min_value = i.x;
            _r.settings().scalar_property_range.max_value = i.y;
        }
        else if constexpr(std::is_same_v<ST,float> || std::is_same_v<ST,double>) {
            Vec2f f = {_r.settings().scalar_property_range.min_value,_r.settings().scalar_property_range.max_value};
            ImGui::SliderFloat2("Range", &f.x, min, max);
            _r.settings().scalar_property_range.min_value = f.x;
            _r.settings().scalar_property_range.max_value = f.y;
        } else if constexpr(std::is_same_v<ST,bool>) {
            bool b_show_false = !_r.settings().scalar_property_range.min_value;
            bool b_show_true = _r.settings().scalar_property_range.max_value;
            ImGui::Checkbox("False", &b_show_false);
            ImGui::Checkbox("True", &b_show_true);
            _r.settings().scalar_property_range.min_value = !b_show_false;
            _r.settings().scalar_property_range.max_value = b_show_true;
        }

        ImGui::ColorEdit3("Min Color", &_r.settings().scalar_property_range.min_color[0]);
        ImGui::ColorEdit3("Max Color", &_r.settings().scalar_property_range.max_color[0]);
    }

    std::string name() override {
        return "Scalar Range";
    }

    ST min;
    ST max;
};

template<typename ST>
struct ScalarPropertyExactFilter : public PropertyFilterBase
{
    using Scalar = ST;

    void renderUI(GL::MeshRenderer& _r) override
    {
        //_r.settings().v_prop_range
        if constexpr(std::is_same_v<ST,int>) {
            int i = _r.settings().scalar_property_range.min_value;
            ImGui::InputInt("Value", &i);
            _r.settings().scalar_property_range.min_value = i;
            _r.settings().scalar_property_range.max_value = i;
        }
        else if constexpr(std::is_same_v<ST,float> || std::is_same_v<ST,double>) {
            float f = _r.settings().scalar_property_range.min_value;
            ImGui::InputFloat("Value", &f);
            _r.settings().scalar_property_range.min_value = f;
            _r.settings().scalar_property_range.max_value = f;
        }
        else if constexpr(std::is_same_v<ST,bool>) {
            bool b = _r.settings().scalar_property_range.min_value;
            ImGui::Checkbox("True", &b);
            _r.settings().scalar_property_range.min_value = b;
            _r.settings().scalar_property_range.max_value = b;
        }
        ImGui::ColorEdit3("Color", &_r.settings().scalar_property_range.min_color[0]);
        _r.settings().scalar_property_range.max_color = _r.settings().scalar_property_range.min_color;
    }

    std::string name() override {
        return "Exact Scalar";
    }
};

}
