find_path(ASIO_INCLUDE_DIR
    NAMES asio.hpp
    PATHS
        /opt/homebrew/include
        /usr/local/include
        /usr/include
    NO_DEFAULT_PATH
)
find_path(ASIO_INCLUDE_DIR NAMES asio.hpp)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Asio
    REQUIRED_VARS ASIO_INCLUDE_DIR
)

if(Asio_FOUND AND NOT TARGET asio::asio)
    add_library(asio::asio INTERFACE IMPORTED)
    set_target_properties(asio::asio PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${ASIO_INCLUDE_DIR}"
    )
endif()

mark_as_advanced(ASIO_INCLUDE_DIR)
