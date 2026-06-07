add_library(gx_scripting STATIC
    LuaEngine.cpp
)

target_include_directories(gx_scripting PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(gx_scripting PRIVATE
    sol2::sol2
    spdlog::spdlog
    fmt::fmt
)
