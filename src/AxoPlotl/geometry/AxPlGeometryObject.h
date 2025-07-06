#pragma once

#include "AxoPlotl/geometry/presets.h"
#include "AxoPlotl/rendering/Renderer.h"
#include "AxoPlotl/utils/Color.h"
#include "AxoPlotl/utils/FileAccessor.h"
#include <string>

namespace AxoPlotl
{

class AxPlGeometryObject
{
protected:
    int id_;
    static int id_counter_;
    std::string type_name_ = "AxPlObject";;
    std::string name_ = "BASE";
    Color ui_color_;
    bool visible_ = true;
    bool deleted_ = false;
    std::vector<Renderer::BatchLocation> renderLocation_;

    void startRenderUI();

    virtual void updateRenderUI() = 0;

    void endRenderUI();

public:
    AxPlGeometryObject(const std::string& _type_name, const std::string& _name) :
        id_(++id_counter_), type_name_(_type_name), name_(_name), ui_color_(Color::random())
    {}

    inline void renderUI() {
        startRenderUI();
        updateRenderUI();
        endRenderUI();
    }
};

class EmptyObject : public AxPlGeometryObject
{
public:
    EmptyObject()  : AxPlGeometryObject("Empty", "NULL") {}

    inline void updateRenderUI() override {}
};

class TetrahedralMeshObject : public AxPlGeometryObject
{
private:
    std::string filepath_;
    TetrahedralMesh mesh_;

public:
    TetrahedralMeshObject(const std::string& _filepath) :
        AxPlGeometryObject("Tet Mesh", std::filesystem::path(_filepath).filename()),
        filepath_(_filepath)
    {
        IO::readMesh(filepath_, mesh_);
    }

    void updateRenderUI() override;
};

class ExplicitSurfaceObject : public AxPlGeometryObject
{
private:
    ExplicitSurfaceFunction f_;
    ColorFunction2f color_;
    uint resolution_ = 32;
    char input_buffer_[256];

public:
    ExplicitSurfaceObject(const std::string& _name, const ExplicitSurfaceFunction& _f, Color _color = Color::BLUE) :
        AxPlGeometryObject("Explicit Surface", _name),  f_(_f), color_(_color)
    {}

    void updateRenderUI() override;

};

class ImplicitSurfaceObject : public AxPlGeometryObject
{
private:
    std::function<float(float,float,float)> f_;
    Color color_;
    uint resolution_ = 32;

public:
    ImplicitSurfaceObject(const std::function<float(float,float,float)>& _f, Color _color = Color::RED) :
        AxPlGeometryObject("Implicit Surface", "NONAME"), f_(_f), color_(_color)
    {}

    void updateRenderUI() override;

};

}
