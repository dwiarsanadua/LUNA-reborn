# Try to find imgui; if not found, create a dummy target
# The map_editor tool needs this; other projects don't.
find_path(imgui_INCLUDE_DIR
    NAMES imgui.h
    PATHS /opt/homebrew/include /usr/local/include /usr/include
    PATH_SUFFIXES imgui
)

find_library(imgui_LIBRARY
    NAMES imgui
    PATHS /opt/homebrew/lib /usr/local/lib /usr/lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(imgui DEFAULT_MSG imgui_INCLUDE_DIR imgui_LIBRARY)

if(imgui_FOUND AND NOT TARGET imgui::imgui)
    add_library(imgui::imgui UNKNOWN IMPORTED)
    set_target_properties(imgui::imgui PROPERTIES
        IMPORTED_LOCATION "${imgui_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${imgui_INCLUDE_DIR}"
    )
elseif(NOT TARGET imgui::imgui)
    add_library(imgui::imgui INTERFACE IMPORTED)
    message(WARNING "imgui not found; map_editor will not compile")
endif()
