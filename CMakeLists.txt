cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(pcprep C CXX)

# Optional libraries
set(OPTIONAL_LIBRARIES 
    GL
    glfw
    GLEW
    png
)

# Create options for each library
foreach(lib ${OPTIONAL_LIBRARIES})
    option(WITH_${lib} "Build with ${lib} support" OFF)
endforeach()

option(WITH_EXAMPLES "Build examples" ON)

include_directories(${PROJECT_SOURCE_DIR}/include)

# Collect source files
file(GLOB PCPREP_SHARED_SOURCE 
    ${PROJECT_SOURCE_DIR}/src/pcprep/*.c 
    ${PROJECT_SOURCE_DIR}/src/pcprep/*.cpp
    ${PROJECT_SOURCE_DIR}/src/cJSON/*.c
    ${PROJECT_SOURCE_DIR}/src/miniply/*.cpp
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DABC")
set(CMAKE_EXE_LINKER_FLAGS "")

# Function to handle optional library linking
function(link_optional_libraries target)
    foreach(lib ${OPTIONAL_LIBRARIES})
        if(WITH_${lib})
            target_link_libraries(${target} ${lib})
            string(TOUPPER ${lib} LIBFLAG)
            target_compile_definitions(${target} PRIVATE WITH_${LIBFLAG})
        else()
            message(STATUS "${lib} support is disabled")
        endif()
    endforeach()
endfunction()

# Main executable function
function(add_pcprep_executable target_name source_dir source_file link_optional_libs)
    set(SOURCE_FILES ${PCPREP_SHARED_SOURCE} ${source_dir}/${source_file})
    add_executable(${target_name} ${SOURCE_FILES})
    set_target_properties(${target_name} PROPERTIES 
        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
    )

    if(link_optional_libs)
        link_optional_libraries(${target_name})
    endif()
endfunction()

# Add executables
add_pcprep_executable(pcp ${PROJECT_SOURCE_DIR}/pcp pcp.c ON)

# Add example executables
if(WITH_EXAMPLES)
    add_pcprep_executable(cam2mat ${PROJECT_SOURCE_DIR}/examples/cam2mat cam2mat.cpp OFF)
    add_pcprep_executable(pc_io ${PROJECT_SOURCE_DIR}/examples/pc_io pc_io.c OFF)
    add_pcprep_executable(tiling ${PROJECT_SOURCE_DIR}/examples/tiling tiling.c OFF)
    add_pcprep_executable(merge ${PROJECT_SOURCE_DIR}/examples/merge merge.c OFF)
    add_pcprep_executable(subsampling ${PROJECT_SOURCE_DIR}/examples/subsampling subsampling.c OFF)
    add_pcprep_executable(screen_area ${PROJECT_SOURCE_DIR}/examples/screen_area screen_area.cpp OFF)
endif()