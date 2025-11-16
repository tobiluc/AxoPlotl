#pragma once

#include <polyscope/quantity.h>

namespace polyscope
{

class PolyscopeOVM; // forward declare

class PolyscopeOVMQuantity : public QuantityS<PolyscopeOVM> {
public:
    PolyscopeOVMQuantity(std::string name, PolyscopeOVM& parentStructure, bool dominates = false);
    ~PolyscopeOVMQuantity() {};
    virtual std::shared_ptr<render::ShaderProgram> createSliceProgram() { return nullptr; };
    virtual void drawSlice(polyscope::SlicePlane* sp) {};

public:
    virtual void buildVertexInfoGUI(size_t vInd);
    virtual void buildEdgeInfoGUI(size_t eInd);
    virtual void buildFaceInfoGUI(size_t fInd);
    virtual void buildCellInfoGUI(size_t cInd);
};


}
