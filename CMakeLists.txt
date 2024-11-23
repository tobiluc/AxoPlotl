cmake_minimum_required(VERSION 3.18 FATAL_ERROR)
cmake_policy(VERSION 3.18.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(MeshViewer VERSION 0.1.0 LANGUAGES C CXX)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wa,-mbig-obj")
endif()


file(GLOB_RECURSE SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
add_executable(MeshViewer ${SRC_FILES} ${HEADER_FILES})

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
    GIT_TAG        3.3.8 # Replace with the desired GLFW version
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
        GIT_TAG        0.9.9.8 # Replace with the desired GLM version
        SOURCE_DIR "${EXTERNAL_DIR}/glm"
    )
    FetchContent_MakeAvailable(glm)
endif()

#=========================
# Glad and OpenGL
#=========================
add_subdirectory(external/glad/)
find_package(OpenGL REQUIRED) # Link OpenGL (on most systems)

#=========================
# ImGui
#=========================
# if (NOT TARGET imgui)
#     message(STATUS "Fetching ImGui")
#     FetchContent_Declare(imgui
#         GIT_REPOSITORY https://github.com/ocornut/imgui.git
#         SOURCE_DIR "${EXTERNAL_DIR}/imgui"
#     )
#     FetchContent_MakeAvailable(imgui)
# endif()
#find_package(imgui REQUIRED)
#set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/imgui")
#target_include_directories(MeshViewer PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/imgui")
#target_link_directories(MeshViewer PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/imgui")
# add_library(imgui
#     ${IMGUI_DIR}/imgui.cpp
#     ${IMGUI_DIR}/imgui_draw.cpp
#     ${IMGUI_DIR}/imgui_tables.cpp
#     ${IMGUI_DIR}/imgui_widgets.cpp
#     ${IMGUI_DIR}/imgui_impl_glfw.cpp
#     ${IMGUI_DIR}/imgui_impl_opengl3.cpp
# )
# target_include_directories(imgui PUBLIC
#     ${IMGUI_DIR}
#     #${EXTERNAL_DIR}
#     #${GLFW_INCLUDE_DIR}
# )
#target_link_libraries(imgui PRIVATE glfw OpenGL::GL)

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
#imgui
)

#=========================
# Testing
#=========================
include(CTest)

enable_testing()

