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
add_executable(MeshViewer
${SRC_FILES}
)
add_custom_target(HeaderFiles SOURCES ${HEADER_FILES})

#=========================
# GLFW
#=========================
include(FetchContent) 
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
set(EXTERNAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external")

if (NOT TARGET glfw)
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
    FetchContent_Declare(glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG        0.9.9.8 # Replace with the desired GLM version
        SOURCE_DIR "${EXTERNAL_DIR}/glm"
    )
    FetchContent_MakeAvailable(glm)
endif()

#=========================
# OVM
#=========================
if (NOT TARGET OpenVolumeMesh::OpenVolumeMesh)
    FetchContent_Declare(ovm
        GIT_REPOSITORY https://www.graphics.rwth-aachen.de:9000/OpenVolumeMesh/OpenVolumeMesh
        GIT_TAG v3.3
        SOURCE_DIR "${EXTERNAL_DIR}/OpenVolumeMesh"
        )
    FetchContent_MakeAvailable(ovm)
endif()


#=========================
# Glad and OpenGL
#=========================
add_subdirectory(external/glad/)
find_package(OpenGL REQUIRED) # Link OpenGL (on most systems)

#=========================
# Link
#=========================
target_link_libraries(MeshViewer
glad
glfw 
OpenGL::GL
glm
OpenVolumeMesh::OpenVolumeMesh
)

#=========================
# Testing
#=========================
include(CTest)

enable_testing()

