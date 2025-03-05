#!/bin/bash

# Default build options
BUILD_DIR="build"
WITH_EXAMPLES=OFF
WITH_LIBS=()

rm -r "$BUILD_DIR"
# Extract optional libraries from CMake cache
mkdir -p "$BUILD_DIR" && cd "$BUILD_DIR"
cmake .. > /dev/null 2>&1
ALL_LIBS=($(cmake -L .. | grep -oP '^with_\K[^:]+' | tr '\n' ' '))
cd ..

DESKTOP_LIBS=(GL glfw GLEW)  # Libraries required for desktop builds

    show_help() {
        echo "Usage: ./build.sh [options]"
        echo "Options:"
        echo "  --examples        Enable example builds"
        echo "  --with-<lib>      Enable specific optional library (e.g., --with-GL)"
        echo "  --all             Enable all optional libraries"
        echo "  --desktop         Enable desktop-related libraries (GL, glfw, GLEW)"
        echo "  --clean           Remove build and install directories"
        echo "  --help            Show this help message"
        echo "Available optional libraries:"
        for lib in "${ALL_LIBS[@]}"; do
            echo "  - $lib"
        done
        exit 0
    }

# Parse command-line arguments
while [[ "$#" -gt 0 ]]; do
    case "$1" in
        --examples)
            WITH_EXAMPLES=ON
            ;;
        --with-*)
            LIB_NAME=${1#--with-}  # Extract library name
            WITH_LIBS+=("-Dwith_${LIB_NAME}=ON")
            ;;
        --all)
            for lib in "${ALL_LIBS[@]}"; do
                WITH_LIBS+=("-Dwith_${lib}=ON")
            done
            ;;
        --desktop)
            for lib in "${DESKTOP_LIBS[@]}"; do
                WITH_LIBS+=("-Dwith_${lib}=ON")
            done
            ;;
        --clean)
            echo "Cleaning build directory..."
            rm -rf "$BUILD_DIR"
            exit 0
            ;;
        --help)
            show_help
            ;;
        *)
            echo "Unknown option: $1"
            show_help
            ;;
    esac
    shift
done

# Create and enter build directory
mkdir -p "$BUILD_DIR" && cd "$BUILD_DIR"

# Run CMake configuration
cmake .. -Dexamples=$WITH_EXAMPLES "${WITH_LIBS[@]}"

# Build the project
make -j$(nproc)

