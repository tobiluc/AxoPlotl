cmake_minimum_required(VERSION 3.18 FATAL_ERROR)
cmake_policy(VERSION 3.18.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(AxoPlotl VERSION 0.1.0 LANGUAGES C CXX)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wa,-mbig-obj")
endif()

#file(GLOB_RECURSE SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/AxoPlotl/*.cpp")
#file(GLOB_RECURSE HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/AxoPlotl/*.h")
add_executable(${PROJECT_NAME}
    src/AxoPlotl/main.cpp

    src/AxoPlotl/AxoPlotl.h
    src/AxoPlotl/AxoPlotl.cpp

    src/AxoPlotl/commons/Camera.h
    src/AxoPlotl/commons/Camera.cpp
    src/AxoPlotl/commons/PickingTexture.h
    src/AxoPlotl/commons/PickingTexture.cpp
    src/AxoPlotl/commons/Shader.h
    src/AxoPlotl/commons/Shader.cpp
    src/AxoPlotl/commons/Texture.h
    src/AxoPlotl/commons/Texture.cpp



    src/AxoPlotl/rendering/GLBuffers.h
    src/AxoPlotl/rendering/ImGuiRenderer.h
    src/AxoPlotl/rendering/ImGuiRenderer.cpp

    src/AxoPlotl/rendering/TrianglesRenderBatch.h
    src/AxoPlotl/rendering/TrianglesRenderBatch.cpp
    src/AxoPlotl/rendering/LinesRenderBatch.h
    src/AxoPlotl/rendering/LinesRenderBatch.cpp
    src/AxoPlotl/rendering/PointsRenderBatch.h
    src/AxoPlotl/rendering/PointsRenderBatch.cpp
    src/AxoPlotl/rendering/MeshRenderer.h


    src/AxoPlotl/utils/Time.h
    src/AxoPlotl/utils/Time.cpp
    src/AxoPlotl/utils/KeyHandler.h
    src/AxoPlotl/utils/MouseHandler.h
    src/AxoPlotl/utils/MouseHandler.cpp
    src/AxoPlotl/algorithms/simplex_noise.h
    src/AxoPlotl/algorithms/simplex_noise.cpp
    src/AxoPlotl/commons/stb_image.h
    src/AxoPlotl/utils/Typedefs.h
    src/AxoPlotl/utils/Utils.h
    src/AxoPlotl/utils/Utils.cpp


    src/AxoPlotl/rendering/Renderer.h
    src/AxoPlotl/rendering/Renderer.cpp
    src/AxoPlotl/rendering/RenderBatch.h
    src/AxoPlotl/rendering/RenderBatch.cpp

    src/AxoPlotl/commons/Color.h
    src/AxoPlotl/commons/Color.cpp
    src/AxoPlotl/parsing/tokens.h
    src/AxoPlotl/parsing/tokens.cpp
    src/AxoPlotl/parsing/tree.h
    src/AxoPlotl/parsing/parsing.h
    src/AxoPlotl/parsing/parsing.cpp


    src/AxoPlotl/parsing/Scope.h
    src/AxoPlotl/parsing/Scope.cpp
    src/AxoPlotl/parsing/tree.cpp


    src/AxoPlotl/algorithms/marching_cubes.h
    src/AxoPlotl/algorithms/marching_cubes.cpp
    src/AxoPlotl/Scene.h
    src/AxoPlotl/Scene.cpp
    src/AxoPlotl/geometry/Surface.h
    src/AxoPlotl/geometry/Surface.cpp
    src/AxoPlotl/utils/Memory.h
    src/AxoPlotl/utils/Memory.cpp
    src/AxoPlotl/commons/Mesh.h
    src/AxoPlotl/rendering/Primitives.h
    src/AxoPlotl/rendering/Primitives.cpp

    src/AxoPlotl/geometry/Octree.h

    src/AxoPlotl/geometry/Octree.cpp
    src/AxoPlotl/IO/OBJFileAccessor.h
    src/AxoPlotl/IO/OVMFileAccessor.h
    src/AxoPlotl/IO/OVMFileAccessor.cpp
    src/AxoPlotl/IO/FileUtils.h
    src/AxoPlotl/IO/JSONFileAccessor.h
    src/AxoPlotl/geometry/glm.h
    src/AxoPlotl/geometry/ovm.h
    src/AxoPlotl/IO/OBJFileAccessor.cpp
    src/AxoPlotl/IO/PLYFileAccessor.h
    src/AxoPlotl/IO/PLYFileAccessor.cpp
    src/AxoPlotl/IO/FileAccessor.h
    src/AxoPlotl/IO/FileAccessor.cpp
    src/AxoPlotl/geometry/nodes/GeometryNode.h
    src/AxoPlotl/geometry/nodes/GeometryNode.cpp
    src/AxoPlotl/geometry/nodes/MeshNode.h
    src/AxoPlotl/geometry/nodes/MeshNode.cpp
    src/AxoPlotl/geometry/nodes/ExplicitCurveNode.h
    src/AxoPlotl/geometry/nodes/ExplicitCurveNode.cpp
    src/AxoPlotl/geometry/nodes/ExplicitSurfaceNode.h src/AxoPlotl/geometry/nodes/ExplicitSurfaceNode.cpp
    src/AxoPlotl/geometry/nodes/ImplicitSurfaceNode.h src/AxoPlotl/geometry/nodes/ImplicitSurfaceNode.cpp
    src/AxoPlotl/geometry/nodes/ConvexPolygonNode.h src/AxoPlotl/geometry/nodes/ConvexPolygonNode.cpp
    src/AxoPlotl/geometry/nodes/VectorFieldNode.h src/AxoPlotl/geometry/nodes/VectorFieldNode.cpp
    src/AxoPlotl/rendering/MeshRenderer.cpp


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
# NLohmann Json
#=========================
if (NOT TARGET nlohmann_json::nlohmann_json)
    FetchContent_Declare(
        json
        URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
    )
    FetchContent_MakeAvailable(json)
endif()

#=========================
# Link
#=========================
target_link_libraries(${PROJECT_NAME}
glad
glfw 
OpenGL::GL
glm
OpenVolumeMesh::OpenVolumeMesh
ImGui
ImGuiFileDialog
nlohmann_json::nlohmann_json
)
target_link_libraries(ImGuiFileDialog ImGui)

#=========================
# Define what to include
#=========================
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/src>
)

#=========================
# Testing
#=========================
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)
add_executable(${PROJECT_NAME}-Tests tests/tests.cpp)
target_link_libraries(${PROJECT_NAME}-Tests PRIVATE
    gtest
    gtest_main
    glad
    glfw
    OpenGL::GL
    glm
    OpenVolumeMesh::OpenVolumeMesh
    ImGui
    ImGuiFileDialog
    nlohmann_json::nlohmann_json
)
target_include_directories(${PROJECT_NAME}-Tests PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/src>
)
add_test(NAME MyGoogleTest COMMAND test_runner)
