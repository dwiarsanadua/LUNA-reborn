#!/bin/bash
# CI script to build bgfx from source
set -e

BGFX_DIR="${1:-external/bgfx}"
BUILD_DIR="$BGFX_DIR/.build/ci"

echo "Building bgfx in $BGFX_DIR..."

if [ ! -d "$BGFX_DIR" ]; then
    echo "bgfx directory not found at $BGFX_DIR"
    echo "Skipping bgfx build — using system/find_package fallback"
    exit 0
fi

mkdir -p "$BUILD_DIR"
cd "$BGFX_DIR"

# Detect platform
if [[ "$OSTYPE" == "darwin"* ]]; then
    PLATFORM_FLAGS="-DCMAKE_OSX_ARCHITECTURES=arm64"
    RENDERER="-DBGFX_CONFIG_RENDERER_METAL=ON"
elif [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" ]]; then
    PLATFORM_FLAGS=""
    RENDERER="-DBGFX_CONFIG_RENDERER_DIRECT3D11=ON"
else
    PLATFORM_FLAGS=""
    RENDERER="-DBGFX_CONFIG_RENDERER_VULKAN=ON"
fi

cmake -B "$BUILD_DIR" -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    $PLATFORM_FLAGS $RENDERER \
    -DBGFX_BUILD_EXAMPLES=OFF \
    -DBGFX_BUILD_TOOLS=OFF

ninja -C "$BUILD_DIR" bgfx

echo "✅ bgfx built successfully"
