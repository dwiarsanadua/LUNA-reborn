set(BGFX_EXTERNAL_DIR "${CMAKE_SOURCE_DIR}/external")

find_path(bgfx_INCLUDE_DIR
    NAMES bgfx/bgfx.h
    PATHS
        ${BGFX_EXTERNAL_DIR}/bgfx/include
        /opt/homebrew/include
        /usr/local/include
        /usr/include
        $ENV{BGFX_ROOT}/include
    NO_DEFAULT_PATH
)
find_path(bgfx_INCLUDE_DIR NAMES bgfx/bgfx.h)

find_path(bx_INCLUDE_DIR
    NAMES bx/bx.h
    PATHS
        ${BGFX_EXTERNAL_DIR}/bx/include
        /opt/homebrew/include
        /usr/local/include
        /usr/include
    NO_DEFAULT_PATH
)

find_path(bimg_INCLUDE_DIR
    NAMES bimg/bimg.h
    PATHS
        ${BGFX_EXTERNAL_DIR}/bimg/include
        /opt/homebrew/include
        /usr/local/include
        /usr/include
    NO_DEFAULT_PATH
)

find_library(bgfx_LIBRARY
    NAMES bgfxRelease bgfx libbgfxRelease libbgfx
    PATHS
        ${BGFX_EXTERNAL_DIR}/bgfx/.build/osx-arm64/bin
        /opt/homebrew/lib
        /usr/local/lib
        /usr/lib
        $ENV{BGFX_ROOT}/lib
    NO_DEFAULT_PATH
)
find_library(bgfx_LIBRARY NAMES bgfx libbgfx)

find_library(bx_LIBRARY
    NAMES bxRelease bx libbxRelease libbx
    PATHS
        ${BGFX_EXTERNAL_DIR}/bgfx/.build/osx-arm64/bin
        /opt/homebrew/lib
        /usr/local/lib
        /usr/lib
    NO_DEFAULT_PATH
)
find_library(bx_LIBRARY NAMES bx libbx)

find_library(bimg_LIBRARY
    NAMES bimgRelease bimg libbimgRelease libbimg
    PATHS
        ${BGFX_EXTERNAL_DIR}/bgfx/.build/osx-arm64/bin
        /opt/homebrew/lib
        /usr/local/lib
        /usr/lib
    NO_DEFAULT_PATH
)
find_library(bimg_LIBRARY NAMES bimg libbimg)

find_library(bimg_decode_LIBRARY
    NAMES bimg_decodeRelease bimg_decode libbimg_decodeRelease libbimg_decode
    PATHS
        ${BGFX_EXTERNAL_DIR}/bgfx/.build/osx-arm64/bin
    NO_DEFAULT_PATH
)

find_library(bimg_encode_LIBRARY
    NAMES bimg_encodeRelease bimg_encode libbimg_encodeRelease libbimg_encode
    PATHS
        ${BGFX_EXTERNAL_DIR}/bgfx/.build/osx-arm64/bin
    NO_DEFAULT_PATH
)

set(bgfx_INCLUDE_DIRS ${bgfx_INCLUDE_DIR} ${bx_INCLUDE_DIR} ${bimg_INCLUDE_DIR})
set(bgfx_LIBRARIES ${bgfx_LIBRARY} ${bx_LIBRARY} ${bimg_LIBRARY} ${bimg_decode_LIBRARY} ${bimg_encode_LIBRARY})

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
    if(bx_LIBRARY)
        set_property(TARGET bgfx::bgfx APPEND PROPERTY INTERFACE_LINK_LIBRARIES "${bx_LIBRARY}")
    endif()
    if(bimg_LIBRARY)
        set_property(TARGET bgfx::bgfx APPEND PROPERTY INTERFACE_LINK_LIBRARIES "${bimg_LIBRARY}")
    endif()
    if(bimg_decode_LIBRARY)
        set_property(TARGET bgfx::bgfx APPEND PROPERTY INTERFACE_LINK_LIBRARIES "${bimg_decode_LIBRARY}")
    endif()
    if(bimg_encode_LIBRARY)
        set_property(TARGET bgfx::bgfx APPEND PROPERTY INTERFACE_LINK_LIBRARIES "${bimg_encode_LIBRARY}")
    endif()
    if(APPLE)
        set_property(TARGET bgfx::bgfx APPEND PROPERTY
            INTERFACE_LINK_OPTIONS "-framework Cocoa;-framework Metal;-framework MetalKit;-framework QuartzCore;-framework IOKit;-framework CoreGraphics"
        )
    endif()
endif()

mark_as_advanced(bgfx_INCLUDE_DIR bgfx_LIBRARY bx_LIBRARY bimg_LIBRARY)
