#pragma once

#include <filesystem>
#include <iostream>
#include "ImGuiFileDialog.h"

namespace AxoPlotl::Interface
{

inline void imgui_file_dialog_open(const char* _filters, const std::string& _key = "file_dlg_key")
{
    IGFD::FileDialogConfig cfg;
    cfg.path = "..";
    ImGuiFileDialog::Instance()->OpenDialog(
        _key,
        "Choose File",
        _filters,
        cfg);
}

inline std::optional<std::filesystem::path> imgui_file_dialog_get(const std::string& _key = "file_dlg_key")
{
    if (ImGuiFileDialog::Instance()->Display(_key)) {
        if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
            return std::filesystem::path(ImGuiFileDialog::Instance()->GetFilePathName());
        }
        ImGuiFileDialog::Instance()->Close();
    }
    return std::nullopt;
}

}
