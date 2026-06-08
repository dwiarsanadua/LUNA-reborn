# ─── Compiler-specific options ──────────────────────────────────────────────

# Clang / AppleClang (macOS / Linux)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang")
    add_compile_options(
        -Wall -Wextra -Wpedantic
        -Wno-c++98-compat-pedantic
        -Wno-float-equal
        -Wno-missing-prototypes
        -Wno-unused-parameter
        -Wno-unused-variable
        -Wno-unused-but-set-variable
        -fPIC
        -fvisibility=hidden
    )

    if(APPLE)
        add_compile_options(-mmacosx-version-min=14.0 -stdlib=libc++)
    endif()

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g -O0)
    else()
        add_compile_options(-O2 -DNDEBUG)
    endif()
endif()

# MSVC (Microsoft Visual C++ — Windows)
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    # Enable parallel build via /MP
    add_compile_options(/MP /W4)

    # Warning suppressions
    add_compile_options(
        /wd4100  # unreferenced formal parameter
        /wd4189  # local variable initialized but not referenced
        /wd4244  # conversion loss of data
        /wd4267  # conversion from size_t to smaller type
        /wd4456  # declaration hides previous local
        /wd4457  # declaration hides function parameter
        /wd4458  # declaration hides class member
        /wd4459  # declaration hides global declaration
        /wd4996  # deprecated / unsafe function
    )

    # Exception handling
    add_compile_options(/EHsc)

    # Runtime library
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(/MDd /Zi /Od)
        add_link_options(/DEBUG)
    else()
        add_compile_options(/MD /O2 /DNDEBUG)
    endif()

    # LTCG for release builds
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(/GL)
        add_link_options(/LTCG)
    endif()

    message(STATUS "MSVC: parallel build enabled (/MP), warnings level /W4")
endif()

# ─── Windows platform detection ──────────────────────────────────────────
if(WIN32)
    add_definitions(-DWIN32_LEAN_AND_MEAN -DNOMINMAX -DWIN32)
    message(STATUS "Windows platform detected: WIN32 defined, lean/mean headers")
endif()

# ─── Renderer backend selection ──────────────────────────────────────────
# Set via: cmake -DLUNA_RENDERER=D3D12
# Options: Auto, D3D12, Vulkan, Metal, OpenGL
set(LUNA_RENDERER "Auto" CACHE STRING "Graphics backend: Auto, D3D12, Vulkan, Metal, OpenGL")
set_property(CACHE LUNA_RENDERER PROPERTY STRINGS Auto D3D12 Vulkan Metal OpenGL)

if(LUNA_RENDERER STREQUAL "D3D12")
    add_definitions(-DBGFX_CONFIG_RENDERER_DIRECT3D12=1)
    message(STATUS "Renderer: Direct3D 12 (forced)")
elseif(LUNA_RENDERER STREQUAL "Vulkan")
    add_definitions(-DBGFX_CONFIG_RENDERER_VULKAN=1)
    message(STATUS "Renderer: Vulkan (forced)")
elseif(LUNA_RENDERER STREQUAL "Metal")
    add_definitions(-DBGFX_CONFIG_RENDERER_METAL=1)
    message(STATUS "Renderer: Metal (forced)")
elseif(LUNA_RENDERER STREQUAL "OpenGL")
    add_definitions(-DBGFX_CONFIG_RENDERER_OPENGL=1)
    message(STATUS "Renderer: OpenGL (forced)")
else()
    message(STATUS "Renderer: Auto-detected by bgfx")
endif()

# ─── Build type ──────────────────────────────────────────────────────────
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-DDEBUG -D_DEBUG)
else()
    add_compile_options(-DNDEBUG)
endif()

# ─── Shared compile options interface library ────────────────────────────
add_library(luna_compile_options INTERFACE)
target_compile_definitions(luna_compile_options INTERFACE
    $<$<CONFIG:Debug>:DEBUG _DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
    LUNAPLUS_VERSION="${PROJECT_VERSION}"
)

# Export renderer backend as a compile definition for consumer targets
target_compile_definitions(luna_compile_options INTERFACE
    LUNA_RENDERER_BACKEND="${LUNA_RENDERER}"
)
