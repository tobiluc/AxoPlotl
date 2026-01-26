cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
cmake_policy(VERSION 3.5.0)
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

    # src/AxoPlotl/utils/Time.h
    # src/AxoPlotl/utils/Time.cpp

    # src/AxoPlotl/utils/MouseHandler.h
    # src/AxoPlotl/utils/MouseHandler.cpp
    src/AxoPlotl/algorithms/simplex_noise.h
    src/AxoPlotl/algorithms/simplex_noise.cpp
    # src/AxoPlotl/commons/stb_image.h
    # src/AxoPlotl/utils/Utils.h

    src/AxoPlotl/algorithms/marching_cubes.h
    src/AxoPlotl/algorithms/marching_cubes.cpp




    src/AxoPlotl/utils/Memory.h
    src/AxoPlotl/utils/Memory.cpp



    src/AxoPlotl/geometry/Octree.h

    src/AxoPlotl/geometry/Octree.cpp
    src/AxoPlotl/IO/OBJFileAccessor.h
    src/AxoPlotl/IO/OVMFileAccessor.h
    src/AxoPlotl/IO/OVMFileAccessor.cpp
    src/AxoPlotl/IO/FileUtils.h
    src/AxoPlotl/IO/JSONFileAccessor.h
    src/AxoPlotl/commons/glm.h
    src/AxoPlotl/commons/ovm.h
    # src/AxoPlotl/geometry/eigen.h
    src/AxoPlotl/IO/OBJFileAccessor.cpp
    src/AxoPlotl/IO/PLYFileAccessor.h
    src/AxoPlotl/IO/PLYFileAccessor.cpp
    src/AxoPlotl/IO/FileAccessor.h
    src/AxoPlotl/IO/FileAccessor.cpp
    src/AxoPlotl/IO/MESHFileAccessor.h
    src/AxoPlotl/IO/MESHFileAccessor.cpp

    src/AxoPlotl/geometry/AABB.h


    src/AxoPlotl/commons/ovm.cpp


    src/AxoPlotl/interface/imgui_file_dialog.h
    src/AxoPlotl/polyscope_custom_structures/OVMStructure.h
    src/AxoPlotl/polyscope_custom_structures/OVMStructure.cpp
    src/AxoPlotl/polyscope_custom_structures/ExplicitSurfaceStructure.h
    src/AxoPlotl/polyscope_custom_structures/ExplicitSurfaceStructure.cpp
    src/AxoPlotl/polyscope_custom_structures/ImplicitSurfaceStructure.h
    src/AxoPlotl/polyscope_custom_structures/ImplicitSurfaceStructure.cpp
    src/AxoPlotl/experimental/PolyscopeOVM.h
    src/AxoPlotl/experimental/PolyscopeOVM.cpp
    src/AxoPlotl/experimental/PolyscopeOVMQuantity.h
    src/AxoPlotl/experimental/PolyscopeOVMQuantity.cpp
    src/AxoPlotl/experimental/BoolQuantity.h
    src/AxoPlotl/experimental/EdgeBoolQuantity.h
    src/AxoPlotl/experimental/EdgeBoolQuantity.cpp


)

#=========================
# OpenMP
#=========================
# if(APPLE)
#     set(OpenMP_ROOT "/opt/homebrew/opt/libomp") # this is a bit of a hack
# endif()
# find_package(OpenMP COMPONENTS CXX REQUIRED)
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
# Eigen
#=========================
if(NOT TARGET Eigen3::Eigen)
    FetchContent_Declare(Eigen3 SYSTEM
        GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
        GIT_TAG 3.4.0
        GIT_SHALLOW TRUE
        SOURCE_SUBDIR cmake
    )
    FetchContent_MakeAvailable(Eigen3)

    add_library(Eigen3::Eigen INTERFACE IMPORTED)
    set_target_properties(Eigen3::Eigen PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${eigen3_SOURCE_DIR})
    if(OpenMP_CXX_FOUND)
        target_link_libraries(Eigen3::Eigen INTERFACE OpenMP::OpenMP_CXX)
    endif()
endif()

#=========================
# OVM
#=========================
if (NOT TARGET OpenVolumeMesh::OpenVolumeMesh)
    message(STATUS "Fetching OpenVolumeMesh")
    FetchContent_Declare(ovm
        GIT_REPOSITORY https://www.graphics.rwth-aachen.de:9000/OpenVolumeMesh/OpenVolumeMesh
        #GIT_TAG v3.4
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

#=====================
# Polyscope
#=====================
if (NOT TARGET polyscope)
    message(STATUS "Fetching polyscope")
    FetchContent_Declare(polyscope
       GIT_REPOSITORY https://github.com/nmwsharp/polyscope.git
       GIT_TAG        master
       SOURCE_DIR "${EXTERNAL_DIR}/polyscope"
    )
    FetchContent_MakeAvailable(polyscope)
endif()

#=========================
# ibes
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
# ImGui File Dialog
#=========================
if (NOT TARGET ImGuiFileDialog)
    message(STATUS "Fetching ImGuiFileDialog")
    FetchContent_Declare(ImGuiFileDialog
        GIT_REPOSITORY https://github.com/aiekick/ImGuiFileDialog.git
        SOURCE_DIR "${EXTERNAL_DIR}/ImGuiFileDialog"
    )
    FetchContent_MakeAvailable(ImGuiFileDialog)
endif()

# Tell ImGuiFileDialog where to find imgui.h (dependency of polyscope)
target_include_directories(ImGuiFileDialog PRIVATE
    ${polyscope_SOURCE_DIR}/deps/imgui/imgui
)

#=========================
# Link
#=========================
target_link_libraries(${PROJECT_NAME} PUBLIC
#OpenMP::OpenMP_CXX
Eigen3::Eigen
OpenVolumeMesh::OpenVolumeMesh
ImGuiFileDialog
nlohmann_json::nlohmann_json
polyscope
ibex
)

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
    #OpenMP::OpenMP_CXX
    OpenVolumeMesh::OpenVolumeMesh
    ImGuiFileDialog
    nlohmann_json::nlohmann_json
)
target_include_directories(${PROJECT_NAME}-Tests PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/src>
)
add_test(NAME MyGoogleTest COMMAND test_runner)
