cmake_minimum_required(VERSION 3.25)

if(APPLE)
    execute_process(COMMAND xcrun --show-sdk-path
        OUTPUT_VARIABLE CMAKE_OSX_SYSROOT
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    message(STATUS "macOS SDK: ${CMAKE_OSX_SYSROOT}")
    set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "macOS arch")
    set(CMAKE_MACOSX_BUNDLE OFF)
endif()

project(LunaPlusReborn
    VERSION 1.0.0
    LANGUAGES C CXX
    DESCRIPTION "LUNA Online Plus - Reborn"
)

enable_testing()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type")
set(LUNA_GRAPHICS_API "Auto" CACHE STRING "Graphics API: Auto, D3D12, Vulkan, Metal, OpenGL")

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
# Some configs export SQLite::SQLite3 (CMake module), others SQLite3::SQLite3
# (Homebrew/vcpkg config). Provide both names.
if(TARGET SQLite::SQLite3 AND NOT TARGET SQLite3::SQLite3)
    add_library(SQLite3::SQLite3 ALIAS SQLite::SQLite3)
elseif(TARGET SQLite3::SQLite3 AND NOT TARGET SQLite::SQLite3)
    add_library(SQLite::SQLite3 ALIAS SQLite3::SQLite3)
endif()
find_package(Freetype REQUIRED)
find_package(EnTT REQUIRED)
# Homebrew/vcpkg ship "flatbuffers", Debian/Ubuntu ship "FlatBuffers"
find_package(flatbuffers QUIET)
if(NOT flatbuffers_FOUND)
    find_package(FlatBuffers REQUIRED)
endif()
find_package(assimp REQUIRED)
find_package(BZip2 REQUIRED)

add_library(luna_bspatch STATIC
    third_party/bsdiff/bspatch.c
)
target_include_directories(luna_bspatch PUBLIC
    ${CMAKE_SOURCE_DIR}/third_party/bsdiff
)
target_link_libraries(luna_bspatch PUBLIC BZip2::BZip2)

# Asio networking library
set(ASIO_SEARCH_PATHS /usr/local/include /usr/include)
if(APPLE)
    list(PREPEND ASIO_SEARCH_PATHS /opt/homebrew/include)
elseif(WIN32 AND DEFINED ENV{VCPKG_ROOT})
    list(PREPEND ASIO_SEARCH_PATHS "$ENV{VCPKG_ROOT}/installed/x64-windows/include")
endif()
find_path(ASIO_INCLUDE_DIR asio.hpp PATHS ${ASIO_SEARCH_PATHS})
if(ASIO_INCLUDE_DIR)
    add_library(asio::asio INTERFACE IMPORTED)
    set_target_properties(asio::asio PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${ASIO_INCLUDE_DIR}"
    )
    message(STATUS "Found asio: ${ASIO_INCLUDE_DIR}")
elseif(APPLE)
    message(FATAL_ERROR "asio not found. Install with: brew install asio")
elseif(WIN32)
    message(FATAL_ERROR "asio not found. Install with: vcpkg install asio:x64-windows")
else()
    message(FATAL_ERROR "asio not found. Install with: apt install libasio-dev")
endif()

# sol2 Lua scripting (optional)
find_package(sol2 QUIET)
if(NOT sol2_FOUND)
    # Header-only fallback: look for sol/sol.hpp directly
    find_path(SOL2_INCLUDE_DIR sol/sol.hpp PATHS /opt/homebrew/include /usr/local/include /usr/include)
    add_library(sol2::sol2 INTERFACE IMPORTED)
    if(SOL2_INCLUDE_DIR)
        set_target_properties(sol2::sol2 PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${SOL2_INCLUDE_DIR}")
        message(STATUS "Found sol2 headers: ${SOL2_INCLUDE_DIR}")
    else()
        message(WARNING "sol2 not found. Scripting engine disabled. Install with: brew install sol2 (or vcpkg install sol2)")
    endif()
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
