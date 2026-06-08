find_path(Jolt_INCLUDE_DIR
    NAMES Jolt/Jolt.h
    PATHS
        ${CMAKE_SOURCE_DIR}/external/JoltPhysics
        /opt/homebrew/include
        /usr/local/include
        /usr/include
    NO_DEFAULT_PATH
)
find_path(Jolt_INCLUDE_DIR NAMES Jolt/Jolt.h)

find_library(Jolt_LIBRARY
    NAMES Jolt libJolt
    PATHS
        ${CMAKE_SOURCE_DIR}/external/JoltPhysics/Build
        /opt/homebrew/lib
        /usr/local/lib
        /usr/lib
    NO_DEFAULT_PATH
)
find_library(Jolt_LIBRARY NAMES Jolt libJolt)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Jolt
    REQUIRED_VARS Jolt_INCLUDE_DIR Jolt_LIBRARY
)

if(Jolt_FOUND AND NOT TARGET Jolt::Jolt)
    add_library(Jolt::Jolt UNKNOWN IMPORTED)
    set_target_properties(Jolt::Jolt PROPERTIES
        IMPORTED_LOCATION "${Jolt_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${Jolt_INCLUDE_DIR}"
    )
    if(APPLE)
        set_property(TARGET Jolt::Jolt APPEND PROPERTY
            INTERFACE_LINK_OPTIONS "SHELL:-Xlinker -framework -Xlinker Foundation"
        )
    endif()
endif()

mark_as_advanced(Jolt_INCLUDE_DIR Jolt_LIBRARY)
