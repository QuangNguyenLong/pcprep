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

set(PC_IO_DIR ${PROJECT_SOURCE_DIR}/examples/pc_io)
set(PC_IO_SOURCE ${PCPREP_SHARED_SOURCE} ${PC_IO_DIR}/pc_io.c)
add_executable(pc_io ${PC_IO_SOURCE})
# target_link_libraries(pc_io pthread)
set_target_properties(pc_io PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
)

set(TILING_DIR ${PROJECT_SOURCE_DIR}/examples/tiling)
set(TILING_SOURCE ${PCPREP_SHARED_SOURCE} ${TILING_DIR}/tiling.c)
add_executable(tiling ${TILING_SOURCE})
# target_link_libraries(tiling pthread)
set_target_properties(tiling PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
)

set(SUBSAMPLING_DIR ${PROJECT_SOURCE_DIR}/examples/subsampling)
set(SUBSAMPLING_SOURCE ${PCPREP_SHARED_SOURCE} ${SUBSAMPLING_DIR}/subsampling.c)
add_executable(subsampling ${SUBSAMPLING_SOURCE})
# target_link_libraries(subsampling pthread)
set_target_properties(subsampling PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
)

set(SCREEN_AREA_DIR ${PROJECT_SOURCE_DIR}/examples/screen_area)
set(SCREEN_AREA_SOURCE ${PCPREP_SHARED_SOURCE} ${SCREEN_AREA_DIR}/screen_area.cpp)
add_executable(screen_area ${SCREEN_AREA_SOURCE})
# target_link_libraries(screen_area pthread)
set_target_properties(screen_area PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
)
