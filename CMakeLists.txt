cmake_minimum_required(VERSION 3.18 FATAL_ERROR)
cmake_policy(VERSION 3.18.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

project(AxoPlotl VERSION 0.1.0 LANGUAGES C CXX)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wa,-mbig-obj")
endif()

#file(GLOB_RECURSE SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/AxoPlotl/*.cpp")
#file(GLOB_RECURSE HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/AxoPlotl/*.h")
add_executable(${PROJECT_NAME}
    src/AxoPlotl/main.cpp

    src/AxoPlotl/Runner.h
    src/AxoPlotl/Runner.cpp

    src/AxoPlotl/commons/Camera.h
    src/AxoPlotl/commons/Camera.cpp
    src/AxoPlotl/commons/stb_image.h
    src/AxoPlotl/rendering/PickingTexture.h
    src/AxoPlotl/rendering/PickingTexture.cpp
    src/AxoPlotl/rendering/Shader.h
    src/AxoPlotl/rendering/Shader.cpp
    src/AxoPlotl/rendering/Texture.h
    src/AxoPlotl/rendering/Texture.cpp

    src/AxoPlotl/rendering/ImGuiRenderer.h
    src/AxoPlotl/rendering/ImGuiRenderer.cpp

    src/AxoPlotl/rendering/MeshRenderer.h

    src/AxoPlotl/utils/Time.h
    src/AxoPlotl/utils/Time.cpp

    src/AxoPlotl/utils/MouseHandler.h
    src/AxoPlotl/utils/MouseHandler.cpp
    src/AxoPlotl/utils/Utils.h


    src/AxoPlotl/commons/Color.h
    src/AxoPlotl/commons/Color.cpp

    src/AxoPlotl/algorithms/marching_cubes.h
    src/AxoPlotl/algorithms/marching_cubes.cpp
    src/AxoPlotl/Scene.h
    src/AxoPlotl/Scene.cpp
    src/AxoPlotl/geometry/Surface.h
    src/AxoPlotl/geometry/Surface.cpp
    src/AxoPlotl/utils/Memory.h
    src/AxoPlotl/utils/Memory.cpp


    src/AxoPlotl/IO/JSONFileAccessor.h

    src/AxoPlotl/IO/FileAccessor.h
    src/AxoPlotl/IO/FileAccessor.cpp
    src/AxoPlotl/geometry/nodes/GeometryNode.h
    src/AxoPlotl/geometry/nodes/GeometryNode.cpp
    src/AxoPlotl/geometry/nodes/MeshNode.h
    src/AxoPlotl/geometry/nodes/MeshNode.cpp
    src/AxoPlotl/geometry/nodes/ExplicitCurveNode.h
    src/AxoPlotl/geometry/nodes/ExplicitCurveNode.cpp
    src/AxoPlotl/geometry/nodes/ExplicitSurfaceNode.h
    src/AxoPlotl/geometry/nodes/ExplicitSurfaceNode.cpp
    src/AxoPlotl/geometry/nodes/ImplicitSurfaceNode.h
    src/AxoPlotl/geometry/nodes/ImplicitSurfaceNode.cpp
    src/AxoPlotl/geometry/nodes/ConvexPolygonNode.h
    src/AxoPlotl/geometry/nodes/ConvexPolygonNode.cpp
    src/AxoPlotl/geometry/nodes/VectorFieldNode.h
    src/AxoPlotl/geometry/nodes/VectorFieldNode.cpp
    src/AxoPlotl/rendering/MeshRenderer.cpp
    src/AxoPlotl/geometry/nodes/SphericalHarmonicNode.h
    src/AxoPlotl/geometry/nodes/SphericalHarmonicNode.cpp

    src/AxoPlotl/rendering/redraw_frames.h
    src/AxoPlotl/rendering/redraw_frames.cpp
    src/AxoPlotl/properties/property_filters.hpp
    src/AxoPlotl/properties/property_data.hpp



    src/AxoPlotl/typedefs/typedefs_OpenVolumeMesh.hpp
    src/AxoPlotl/geometry/AABB.hpp
    src/AxoPlotl/typedefs/typedefs_glm.hpp
    src/AxoPlotl/typedefs/typedefs_ToLoG.hpp
    src/AxoPlotl/utils/string_format.hpp




)

#=========================
# OpenMP
#=========================
if(APPLE)
    set(OpenMP_ROOT "/opt/homebrew/opt/libomp") # this is a bit of a hack
endif()
find_package(OpenMP COMPONENTS CXX REQUIRED)
# if(OpenMP_CXX_FOUND)
#     target_compile_options(${PROJECT_NAME} PUBLIC ${OpenMP_CXX_FLAGS})
#     target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_CXX)
# endif()

#=========================
# Init Fetch Content
#=========================
include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
set(SUBMODULES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/submodules")
set(EXTERNAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external")

#=========================
# ToLoG
#=========================
if (NOT TARGET ToLoG)
    message(STATUS "Fetching ToLoG")
    FetchContent_Declare(ToLoG
        GIT_REPOSITORY https://github.com/tobiluc/ToLoG.git
        GIT_TAG main
        SOURCE_DIR "${EXTERNAL_DIR}/ToLoG"
    )
    set(TOLOG_BUILD_UNIT_TESTS FALSE)
    FetchContent_MakeAvailable(ToLoG)
endif()

#=========================
# Eigen
#=========================
# if(NOT TARGET Eigen3::Eigen)
#     FetchContent_Declare(Eigen3 SYSTEM
#         GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
#         GIT_TAG 3.4.0
#         GIT_SHALLOW TRUE
#         SOURCE_SUBDIR cmake
#     )
#     FetchContent_MakeAvailable(Eigen3)

#     add_library(Eigen3::Eigen INTERFACE IMPORTED)
#     set_target_properties(Eigen3::Eigen PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${eigen3_SOURCE_DIR})
#     if(OpenMP_CXX_FOUND)
#         target_link_libraries(Eigen3::Eigen INTERFACE OpenMP::OpenMP_CXX)
#     endif()
# endif()

#=========================
# ibex
#=========================
if (NOT TARGET ibex)
    message(STATUS "Fetching ibex")
    FetchContent_Declare(ibex
        GIT_REPOSITORY https://github.com/tobiluc/ibex.git
        GIT_TAG main
        SOURCE_DIR "${EXTERNAL_DIR}/ibex"
    )
    FetchContent_MakeAvailable(ibex)
endif()

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
# OpenVolumeMesh
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
# OpenMesh
#=========================
# if (NOT TARGET OpenMeshCore)
#    message(STATUS "Fetching OpenMesh")
#    FetchContent_Declare(om
#        GIT_REPOSITORY https://www.graphics.rwth-aachen.de:9000/OpenMesh/OpenMesh
#        GIT_TAG "OpenMesh-11.0"
#        SOURCE_DIR "${EXTERNAL_DIR}/OpenMesh"
#        )
#    FetchContent_MakeAvailable(om)
# endif()

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
target_link_libraries(${PROJECT_NAME} PUBLIC
glad
glfw 
OpenGL::GL
OpenMP::OpenMP_CXX
glm
# Eigen3::Eigen
# OpenMeshCore
OpenVolumeMesh::OpenVolumeMesh
ImGui
ImGuiFileDialog
nlohmann_json::nlohmann_json
ibex
ToLoG
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
    OpenMP::OpenMP_CXX
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
