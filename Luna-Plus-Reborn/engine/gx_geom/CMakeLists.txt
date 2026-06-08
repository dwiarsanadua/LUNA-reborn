add_library(gx_geom STATIC
    Model.cpp
    MeshObject.cpp
    AnimationSystem.cpp
    Skeleton.cpp
)

target_include_directories(gx_geom PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include/external
    /opt/homebrew/include
)

target_link_libraries(gx_geom PUBLIC
    luna_compile_options
    bgfx::bgfx
    spdlog::spdlog
    fmt::fmt
    glm::glm
    assimp::assimp
)
