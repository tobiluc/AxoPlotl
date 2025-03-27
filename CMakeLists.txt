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
    src/AxoPlotl/commons/shader.cpp
    src/AxoPlotl/commons/Texture.h
    src/AxoPlotl/commons/Texture.cpp

    src/AxoPlotl/geometry/AxPlInput.h

    src/AxoPlotl/rendering/GLBuffers.h
    src/AxoPlotl/rendering/ImGuiRenderer.h
    src/AxoPlotl/rendering/ImGuiRenderer.cpp

    src/AxoPlotl/rendering/TrianglesRenderBatch.h
    src/AxoPlotl/rendering/TrianglesRenderBatch.cpp
    src/AxoPlotl/rendering/LinesRenderBatch.h
    src/AxoPlotl/rendering/LinesRenderBatch.cpp
    src/AxoPlotl/rendering/PointsRenderBatch.h
    src/AxoPlotl/rendering/PointsRenderBatch.cpp

    src/AxoPlotl/utils/FileAccessor.h
    src/AxoPlotl/utils/FileAccessor.cpp
    src/AxoPlotl/utils/Time.h
    src/AxoPlotl/utils/Time.cpp
    src/AxoPlotl/utils/KeyHandler.h
    src/AxoPlotl/utils/MouseHandler.h
    src/AxoPlotl/utils/MouseHandler.cpp
    src/AxoPlotl/utils/simplex_noise.h
    src/AxoPlotl/utils/simplex_noise.cpp
    src/AxoPlotl/utils/stb_image.h
    src/AxoPlotl/utils/Typedefs.h
    src/AxoPlotl/utils/Utils.h
    src/AxoPlotl/utils/Utils.cpp


    src/AxoPlotl/rendering/Renderer.h
    src/AxoPlotl/rendering/Renderer.cpp
    src/AxoPlotl/rendering/RenderBatch.h
    src/AxoPlotl/rendering/RenderBatch.cpp

    src/AxoPlotl/utils/Color.h
    src/AxoPlotl/utils/Color.cpp
    src/AxoPlotl/parsing/tokens.h
    src/AxoPlotl/parsing/tokens.cpp
    src/AxoPlotl/parsing/tree.h
    src/AxoPlotl/parsing/parsing.h
    src/AxoPlotl/parsing/parsing.cpp
    src/AxoPlotl/geometry/AxPlInput.cpp
    src/AxoPlotl/parsing/values.h
    src/AxoPlotl/parsing/Scope.h
    src/AxoPlotl/parsing/Scope.cpp
    src/AxoPlotl/parsing/tree.cpp

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
target_link_libraries(${PROJECT_NAME}
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
)
target_include_directories(${PROJECT_NAME}-Tests PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/src>
)
add_test(NAME MyGoogleTest COMMAND test_runner)
