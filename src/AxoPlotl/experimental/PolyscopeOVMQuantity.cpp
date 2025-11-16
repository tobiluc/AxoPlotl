#include "PolyscopeOVMQuantity.h"
#include "PolyscopeOVM.h"

namespace polyscope
{
PolyscopeOVMQuantity::PolyscopeOVMQuantity(std::string name, PolyscopeOVM& parentStructure, bool dominates)
    : QuantityS<PolyscopeOVM>(name, parentStructure, dominates) {}

void PolyscopeOVMQuantity::buildVertexInfoGUI(size_t vInd) {}
void PolyscopeOVMQuantity::buildFaceInfoGUI(size_t fInd) {}
void PolyscopeOVMQuantity::buildEdgeInfoGUI(size_t eInd) {}
void PolyscopeOVMQuantity::buildCellInfoGUI(size_t cInd) {}
}
