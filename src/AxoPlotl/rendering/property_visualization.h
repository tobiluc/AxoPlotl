#pragma once

#include "AxoPlotl/geometry/ovm.h"
#include "MeshRenderer.h"

namespace AxoPlotl::PropVis
{

// Rules
// List of Rules (Priority)
// Rule: Condition -> Settings
// e.g.
// Bool: True -> Red, False -> Invisible
// Double: Smaller/Greater than... -> ..., Color Gradient within Range
// Int

/// Computes min, max of a scalar property
template<typename T, typename Handle, typename Entity>
std::pair<T,T> computeRange(const PolyhedralMesh& mesh, OVM::PropertyPtr<T,Entity>& prop)
{
    T min = std::numeric_limits<T>::infinity();
    T max = -std::numeric_limits<T>::infinity();
    for (auto it = entity_iter<Handle>(mesh); it.is_valid(); ++it) {
        if (prop[*it] < min) {min = prop[*it];}
        if (prop[*it] > max) {max = prop[*it];}
    }
    return {min, max};
}

template<typename T>
struct Predicate {
    std::function<bool(const T&)> func;
    Predicate(const std::function<bool(const T&)>& func) : func(func) {}
    inline bool operator()(const T& t) const {return func(t);}
};

template<typename Handle>
struct Action {
    std::function<void(Handle, GL::MeshRenderer::Data&)> func;
    Action(const std::function<void(Handle, GL::MeshRenderer::Data&)>& func) : func(func) {}
    inline void operator()(Handle h, GL::MeshRenderer::Data& data) const {func(h, data);}
};

template<typename T, typename Handle>
struct Rule {
    Predicate<T> predicate;
    Action<Handle> action;
    Rule(const Predicate<T>& p, const Action<Handle> a) : predicate(p), action(a) {}
};

template<typename T, typename Handle, typename Entity>
void generateRenderData(const PolyhedralMesh& mesh, OVM::PropertyPtr<T,Entity>& prop,
    const std::vector<Rule<T,Handle>>& rules
    , GL::MeshRenderer::Data& data)
{
    for (auto it = entity_iter<Handle>(mesh); it.is_valid(); ++it) {
        for (const auto& rule : rules) {
            if ((rule.predicate)(prop[*it])) {
                (rule.action)(*it, data);
                break;
            }
        }
    }
}

}
