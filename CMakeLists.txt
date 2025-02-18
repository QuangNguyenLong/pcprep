cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(pcprep C CXX)

include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB PCPREP_SHARED_SOURCE 
    ${PROJECT_SOURCE_DIR}/src/pcprep/*.c
    ${PROJECT_SOURCE_DIR}/src/pcprep/*.cpp
    ${PROJECT_SOURCE_DIR}/src/miniply/*.cpp
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DABC")
set(CMAKE_EXE_LINKER_FLAGS "-static")

# Function to add executables
function(add_pcprep_executable target_name source_dir source_file)
    set(SOURCE_FILES ${PCPREP_SHARED_SOURCE} ${source_dir}/${source_file})
    add_executable(${target_name} ${SOURCE_FILES})
    set_target_properties(${target_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
    )
endfunction()

# Add executables
# add_pcprep_executable(cam2mat ${PROJECT_SOURCE_DIR}/examples/cam2mat cam2mat.c)
add_pcprep_executable(pc_io ${PROJECT_SOURCE_DIR}/examples/pc_io pc_io.c)
add_pcprep_executable(tiling ${PROJECT_SOURCE_DIR}/examples/tiling tiling.c)
add_pcprep_executable(merge ${PROJECT_SOURCE_DIR}/examples/merge merge.c)
add_pcprep_executable(subsampling ${PROJECT_SOURCE_DIR}/examples/subsampling subsampling.c)
add_pcprep_executable(screen_area ${PROJECT_SOURCE_DIR}/examples/screen_area screen_area.cpp)
add_pcprep_executable(pcp ${PROJECT_SOURCE_DIR}/pcp pcp.c)