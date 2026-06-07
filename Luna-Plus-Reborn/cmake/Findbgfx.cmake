find_path(bgfx_INCLUDE_DIR
    NAMES bgfx/bgfx.h
    PATHS
        /opt/homebrew/include
        /usr/local/include
        /usr/include
        ${CMAKE_SOURCE_DIR}/../external/bgfx/include
        $ENV{BGFX_ROOT}/include
    NO_DEFAULT_PATH
)
find_path(bgfx_INCLUDE_DIR NAMES bgfx/bgfx.h)

find_library(bgfx_LIBRARY
    NAMES bgfxDebug bgfx libbgfxDebug libbgfx
    PATHS
        /opt/homebrew/lib
        /usr/local/lib
        /usr/lib
        ${CMAKE_SOURCE_DIR}/../external/lib
        $ENV{BGFX_ROOT}/lib
    NO_DEFAULT_PATH
)
find_library(bgfx_LIBRARY NAMES bgfx libbgfx)

find_library(bx_LIBRARY
    NAMES bxDebug bx libbxDebug libbx
    PATHS
        /opt/homebrew/lib
        /usr/local/lib
        /usr/lib
        ${CMAKE_SOURCE_DIR}/../external/lib
        $ENV{BGFX_ROOT}/lib
    NO_DEFAULT_PATH
)
find_library(bx_LIBRARY NAMES bx libbx)

find_library(bimg_LIBRARY
    NAMES bimgDebug bimg libbimgDebug libbimg
    PATHS
        /opt/homebrew/lib
        /usr/local/lib
        /usr/lib
        ${CMAKE_SOURCE_DIR}/../external/lib
        $ENV{BGFX_ROOT}/lib
    NO_DEFAULT_PATH
)
find_library(bimg_LIBRARY NAMES bimg libbimg)

set(bgfx_INCLUDE_DIRS ${bgfx_INCLUDE_DIR})
set(bgfx_LIBRARIES ${bgfx_LIBRARY} ${bx_LIBRARY} ${bimg_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(bgfx
    REQUIRED_VARS bgfx_INCLUDE_DIR bgfx_LIBRARY bx_LIBRARY bimg_LIBRARY
)

if(bgfx_FOUND AND NOT TARGET bgfx::bgfx)
    add_library(bgfx::bgfx UNKNOWN IMPORTED)
    set_target_properties(bgfx::bgfx PROPERTIES
        IMPORTED_LOCATION "${bgfx_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${bgfx_INCLUDE_DIRS}"
    )
    if(APPLE)
        set_target_properties(bgfx::bgfx PROPERTIES
            INTERFACE_COMPILE_DEFINITIONS "BGFX_CONFIG_RENDERER_METAL=1"
        )
    endif()
    if(bx_LIBRARY)
        set_property(TARGET bgfx::bgfx APPEND PROPERTY INTERFACE_LINK_LIBRARIES "${bx_LIBRARY}")
    endif()
    if(bimg_LIBRARY)
        set_property(TARGET bgfx::bgfx APPEND PROPERTY INTERFACE_LINK_LIBRARIES "${bimg_LIBRARY}")
    endif()
endif()

mark_as_advanced(bgfx_INCLUDE_DIR bgfx_LIBRARY bx_LIBRARY bimg_LIBRARY)
