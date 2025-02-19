#!/bin/bash

# Options with defaults
WITH_GL=OFF
WITH_GLEW=OFF
WITH_glfw=OFF
WITH_png=OFF
WITH_OSMesa=OFF
WITH_EXAMPLES=OFF

# Parse command-line arguments
while [[ $# -gt 0 ]]; do
  case $1 in
    --with-gl)
      WITH_GL=ON
      shift
      ;;
    --with-glfw)
      WITH_glfw=ON
      shift
      ;;
    --with-osmesa)
      WITH_OSMesa=ON
      shift
      ;;
    --with-glew)
      WITH_GLEW=ON
      shift
      ;;
    --with-png)
      WITH_png=ON
      shift
      ;;
    --with-examples)
      WITH_EXAMPLES=ON
      shift
      ;;
    --all)
      WITH_GL=ON
      WITH_GLEW=ON
      WITH_glfw=ON
      WITH_png=ON
      WITH_OSMesa=ON
      WITH_EXAMPLES=ON
      shift
      ;;
    -h|--help)
      echo "Usage: ./build.sh [options]"
      echo "Options:"
      echo "  --with-gl        Enable OpenGL"
      echo "  --with-glew      Enable GLEW"
      echo "  --with-glfw      Enable glfw"
      echo "  --with-osmesa    Enable OSMesa"
      echo "  --with-png       Enable png"
      echo "  --with-examples  Build examples"
      echo "  --all            Build all"
      echo "  -h, --help       Show this help message"
      exit 0
      ;;
    *)
      echo "Unknown option: $1"
      exit 1
      ;;
  esac
done

# Create build directory
if [ -d "build" ]; then
  echo "Cleaning up existing build directory..."
  rm -rf build
fi
mkdir build
cd build

# Run CMake with optional flags
cmake .. \
  -DWITH_GL=${WITH_GL} \
  -DWITH_GLEW=${WITH_GLEW} \
  -DWITH_glfw=${WITH_glfw} \
  -DWITH_png=${WITH_png} \
  -DWITH_OSMesa=${WITH_OSMesa} \
  -DWITH_EXAMPLES=${WITH_EXAMPLES}

# Build
make -j$(nproc)
