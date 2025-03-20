cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(pcprep C CXX)

option(WITH_GL "Build with OPENGL" OFF)
option(WITH_GLFW "Build with GLFW" OFF)
option(WITH_GLEW "Build with GLEW" OFF)
# option(WITH_PNG "Build with PNG" ON)
option(DESKTOP_MODE "Build desktop mode" OFF)
option(BUILD_EXAMPLES "Build examples" OFF)
option(BUILD_VPCC "Build VPCC" OFF)

include_directories(${PROJECT_SOURCE_DIR}/include)

# Collect source files
file(GLOB PCPREP_SHARED_SOURCE
    ${PROJECT_SOURCE_DIR}/src/pcprep/*.c
    ${PROJECT_SOURCE_DIR}/src/pcprep/*.cpp
    ${PROJECT_SOURCE_DIR}/src/cJSON/*.c
    ${PROJECT_SOURCE_DIR}/src/miniply/*.cpp
)

if(DESKTOP_MODE)
    set(WITH_GL ON)
    set(WITH_GLFW ON)
    set(WITH_GLEW ON)
endif()

add_library(pcprep STATIC ${PCPREP_SHARED_SOURCE})
target_link_libraries(pcprep png)

if(WITH_GL)
    target_link_libraries(pcprep GL)
    target_compile_definitions(pcprep PRIVATE WITH_GL)
    message(STATUS "GL support is enabled")
endif()
if(WITH_GLFW)
    target_link_libraries(pcprep glfw)
    target_compile_definitions(pcprep PRIVATE WITH_GLFW)
    message(STATUS "GLFW support is enabled")
endif()
if(WITH_GLEW)
    target_link_libraries(pcprep GLEW)
    target_compile_definitions(pcprep PRIVATE WITH_GLEW)
    message(STATUS "GLEW support is enabled")
endif()


add_executable(pcp ${PROJECT_SOURCE_DIR}/pcp/pcp.c)
target_link_libraries(pcp pcprep)

if(BUILD_EXAMPLES)
    add_executable(cam2mat ${PROJECT_SOURCE_DIR}/examples/cam2mat/cam2mat.cpp)
    add_executable(pc_io ${PROJECT_SOURCE_DIR}/examples/pc_io/pc_io.c)
    add_executable(tiling ${PROJECT_SOURCE_DIR}/examples/tiling/tiling.c)
    add_executable(merge ${PROJECT_SOURCE_DIR}/examples/merge/merge.c)
    add_executable(subsampling ${PROJECT_SOURCE_DIR}/examples/subsampling/subsampling.c)
    add_executable(screen_area ${PROJECT_SOURCE_DIR}/examples/screen_area/screen_area.cpp)
    target_link_libraries(cam2mat pcprep)
    target_link_libraries(pc_io pcprep)
    target_link_libraries(tiling pcprep)
    target_link_libraries(merge pcprep)
    target_link_libraries(subsampling pcprep)
    target_link_libraries(screen_area pcprep)
endif()

if(BUILD_VPCC)
    include(ExternalProject)
    ExternalProject_Add(
        vpcc
        GIT_REPOSITORY https://github.com/MPEGGroup/mpeg-pcc-tmc2.git
        GIT_TAG master
        CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${PROJECT_SOURCE_DIR}/external/vpcc
        )
endif()

# ------------------------------
# Installation Rules
# ------------------------------

# Install Library
install(TARGETS pcprep
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install Executables
install(TARGETS pcp
    RUNTIME DESTINATION bin
)

if(BUILD_EXAMPLES)
    install(TARGETS cam2mat pc_io tiling merge subsampling screen_area
        RUNTIME DESTINATION bin
    )
endif()

# Install Headers
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/pcprep/ DESTINATION include/pcprep)

