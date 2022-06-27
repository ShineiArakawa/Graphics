cmake_minimum_required(VERSION 3.0.0)
project(Graphics CXX)

set(CMAKE_CXX_FLAGS "-std=c++17")

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

# =========================================================
# Set Libraries ===========================================
# =========================================================
include_directories(
    ${PROJECT_NAME}
    ${PROJECT_SOURCE_DIR}/lib/glad/include
    ${PROJECT_SOURCE_DIR}/lib/glm-0.9.9.8/glm
    ${PROJECT_SOURCE_DIR}/deps/glfw-3.3.7.bin.MACOS/include
)

link_directories(
    ${PROJECT_NAME}
    ${PROJECT_SOURCE_DIR}/deps/glfw-3.3.7.bin.MACOS/lib-x86_64
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_subdirectory(
    src
)
