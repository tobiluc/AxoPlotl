cmake_minimum_required(VERSION 3.18 FATAL_ERROR)
cmake_policy(VERSION 3.18.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(MeshViewer VERSION 0.1.0 LANGUAGES C CXX)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wa,-mbig-obj")
endif()

#file(GLOB_RECURSE SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
#file(GLOB_RECURSE HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
add_executable(MeshViewer
    src/main.cpp

    src/MeshViewer.h
    src/MeshViewer.cpp

    src/commons/Camera.h
    src/commons/Camera.cpp
    src/commons/PickingTexture.h
    src/commons/PickingTexture.cpp
    src/commons/Shader.h
    src/commons/shader.cpp
    src/commons/Texture.h
    src/commons/Texture.cpp

    src/geometry/GeometryObject.h

    src/rendering/GLBuffers.h
    src/rendering/ImGuiRenderer.h
    src/rendering/ImGuiRenderer.cpp
    src/rendering/TetMeshRenderer.h
    src/rendering/TetMeshRenderer.cpp
    src/rendering/RenderBatchTetCells.h
    src/rendering/RenderBatchTetCells.cpp
    src/rendering/TrianglesRenderBatch.h
    src/rendering/TrianglesRenderBatch.cpp
    src/rendering/LinesRenderBatch.h
    src/rendering/LinesRenderBatch.cpp
    src/rendering/PointsRenderBatch.h
    src/rendering/PointsRenderBatch.cpp

    src/utils/FileAccessor.h
    src/utils/FileAccessor.cpp
    src/utils/Globals.h
    src/utils/Globals.cpp
    src/utils/KeyHandler.h
    src/utils/MouseHandler.h
    src/utils/MouseHandler.cpp
    src/utils/simplex_noise.h
    src/utils/simplex_noise.cpp
    src/utils/stb_image.h
    src/utils/Typedefs.h
    src/utils/Utils.h
    src/utils/Utils.cpp
    src/rendering/PolygonRenderer.h
    src/rendering/PolygonRenderer.cpp
    src/rendering/Renderer.h
    src/rendering/Renderer.cpp
    src/rendering/RenderBatch.h
    src/rendering/RenderBatch.cpp

)

#=========================
# GLFW
#=========================
include(FetchContent) 
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
set(EXTERNAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external")

if (NOT TARGET glfw)
    message(STATUS "Fetching glfw")
    set(GLFW_BUILD_EXAMPLES OFF)
    set(GLFW_BUILD_TESTS OFF)
    set(GLFW_BUILD_DOCS OFF)
    set(GLFW_INSTALL OFF)
    FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw
    GIT_TAG        3.3.8
    SOURCE_DIR "${EXTERNAL_DIR}/glfw"
    )
FetchContent_MakeAvailable(glfw)
endif()

#=========================
# GLM
#=========================
if (NOT TARGET glm)
    message(STATUS "Fetching glm")
    FetchContent_Declare(glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG        0.9.9.8
        SOURCE_DIR "${EXTERNAL_DIR}/glm"
    )
    FetchContent_MakeAvailable(glm)
endif()

#=========================
# Glad
#=========================
if (NOT TARGET glad)
    message(STATUS "Fetching glad")
    FetchContent_Declare(glad
        GIT_REPOSITORY https://github.com/Oceanic13/glad.git
        GIT_TAG "main"
        SOURCE_DIR "${EXTERNAL_DIR}/glad"
    )
    FetchContent_MakeAvailable(glad)
endif()

#=========================
# OpenGL
#=========================
find_package(OpenGL REQUIRED)

#=========================
# ImGui
#=========================
if (NOT TARGET ImGui)
    message(STATUS "Fetching ImGui")
    FetchContent_Declare(ImGui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        SOURCE_DIR "${EXTERNAL_DIR}/ImGui"
    )
    FetchContent_MakeAvailable(ImGui)
    add_library(ImGui STATIC
        ${IMGUI_SRC_FILES}
        ${EXTERNAL_DIR}/ImGui/imgui.cpp
        ${EXTERNAL_DIR}/ImGui/imgui_demo.cpp
        ${EXTERNAL_DIR}/ImGui/imgui_draw.cpp
        ${EXTERNAL_DIR}/ImGui/imgui_tables.cpp
        ${EXTERNAL_DIR}/ImGui/imgui_widgets.cpp
        ${EXTERNAL_DIR}/ImGui/backends/imgui_impl_glfw.cpp
        ${EXTERNAL_DIR}/ImGui/backends/imgui_impl_opengl3.cpp
    )
    target_include_directories(ImGui PUBLIC ${EXTERNAL_DIR}/ImGui)
    target_link_libraries(ImGui PUBLIC glfw OpenGL::GL)
endif()
if (NOT TARGET ImGuiFileDialog)
    message(STATUS "Fetching ImGuiFileDialog")
    FetchContent_Declare(ImGuiFileDialog
        GIT_REPOSITORY https://github.com/aiekick/ImGuiFileDialog.git
        SOURCE_DIR "${EXTERNAL_DIR}/ImGuiFileDialog"
    )
    FetchContent_MakeAvailable(ImGuiFileDialog)
endif()


#=========================
# OVM
#=========================
if (NOT TARGET OpenVolumeMesh::OpenVolumeMesh)
    message(STATUS "Fetching OpenVolumeMesh")
    FetchContent_Declare(ovm
        GIT_REPOSITORY https://www.graphics.rwth-aachen.de:9000/OpenVolumeMesh/OpenVolumeMesh
        GIT_TAG v3.3
        SOURCE_DIR "${EXTERNAL_DIR}/OpenVolumeMesh"
        )
    FetchContent_MakeAvailable(ovm)
endif()

#=========================
# Link
#=========================
target_link_libraries(MeshViewer
glad
glfw 
OpenGL::GL
glm
OpenVolumeMesh::OpenVolumeMesh
ImGui
ImGuiFileDialog
)
target_link_libraries(ImGuiFileDialog ImGui)

#=========================
# Testing
#=========================
include(CTest)

enable_testing()

