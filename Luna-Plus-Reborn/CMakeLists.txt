cmake_minimum_required(VERSION 3.30)
project(LunaPlusReborn
    VERSION 1.0.0
    LANGUAGES CXX
    DESCRIPTION "LUNA Online Plus - Reborn"
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "macOS arch")
    set(CMAKE_MACOSX_BUNDLE OFF)
endif()

set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(PkgConfig REQUIRED)

find_package(bgfx REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(Freetype REQUIRED)
find_package(EnTT REQUIRED)
find_package(flatbuffers REQUIRED)
find_package(assimp REQUIRED)

# Asio networking library
find_path(ASIO_INCLUDE_DIR asio.hpp PATHS /opt/homebrew/include /usr/local/include /usr/include)
if(ASIO_INCLUDE_DIR)
    add_library(asio::asio INTERFACE IMPORTED)
    set_target_properties(asio::asio PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${ASIO_INCLUDE_DIR}"
    )
    message(STATUS "Found asio: ${ASIO_INCLUDE_DIR}")
else()
    message(FATAL_ERROR "asio not found. Install with: brew install asio")
endif()

# sol2 Lua scripting (optional)
find_package(sol2 QUIET)
if(NOT sol2_FOUND)
    message(WARNING "sol2 not found. Scripting engine disabled. Install with: brew install sol2")
    add_library(sol2::sol2 INTERFACE IMPORTED)
endif()

# Jolt Physics (optional)
find_package(Jolt QUIET)
if(NOT Jolt_FOUND)
    message(WARNING "Jolt Physics not found. Physics engine will be stubbed.")
    add_library(Jolt::Jolt INTERFACE IMPORTED)
endif()

# libuv (legacy, optional)
find_library(UV_LIBRARY uv PATHS /opt/homebrew/lib /usr/local/lib /usr/lib)
find_path(UV_INCLUDE_DIR uv.h PATHS /opt/homebrew/include /usr/local/include /usr/include)
if(UV_LIBRARY AND UV_INCLUDE_DIR)
    add_library(libuv::uv UNKNOWN IMPORTED)
    set_target_properties(libuv::uv PROPERTIES
        IMPORTED_LOCATION "${UV_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${UV_INCLUDE_DIR}"
    )
    message(STATUS "Found libuv: ${UV_LIBRARY}")
else()
    message(WARNING "libuv not found. Legacy network code disabled.")
    add_library(libuv::uv INTERFACE IMPORTED)
endif()

if(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(METAL_LIBRARY Metal)
    find_library(METALKIT_LIBRARY MetalKit)
    find_library(QUARTZCORE_LIBRARY QuartzCore)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREGRAPHICS_LIBRARY CoreGraphics)
    find_library(CORESERVICES_LIBRARY CoreServices)
    find_library(APPKIT_LIBRARY AppKit)
    set(APPLE_FRAMEWORKS
        ${COCOA_LIBRARY} ${METAL_LIBRARY} ${METALKIT_LIBRARY}
        ${QUARTZCORE_LIBRARY} ${IOKIT_LIBRARY} ${COREGRAPHICS_LIBRARY}
        ${CORESERVICES_LIBRARY} ${APPKIT_LIBRARY}
    )
endif()

include(cmake/CompileOptions.cmake)
include(cmake/ShaderCompile.cmake)

file(GLOB SHADER_SOURCES "shaders/*.sc")
compile_shaders(LunaShaders ${SHADER_SOURCES})

add_subdirectory(engine)

# ─── Client (Phase 0.7: Hello Engine) ────────────────────
add_subdirectory(client)

# ─── Game Library (Phase 1: ECS Core) ──────────────────
add_subdirectory(game)

# ─── Servers (Phase 1) ──────────────────────────────────
add_subdirectory(server)

# ─── Libraries (Phase 5) ────────────────────────────────
add_subdirectory(lib)

# ─── Tools (Phase 3) ─────────────────────────────────────
add_subdirectory(tools)
