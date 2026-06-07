add_library(gx_physics STATIC
    PhysicsWorld.cpp
)

target_include_directories(gx_physics PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include/external
    /opt/homebrew/include
)

target_link_libraries(gx_physics PUBLIC
    luna_compile_options
    Jolt::Jolt
    spdlog::spdlog
    fmt::fmt
)
