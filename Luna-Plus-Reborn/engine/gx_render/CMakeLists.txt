add_library(gx_render STATIC
    RenderDevice.cpp
    Texture.cpp
    TextureManager.cpp
    VertexBuffer.cpp
    Shader.cpp
    Font.cpp
    Sprite3D.cpp
)

target_include_directories(gx_render PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include/external
    /opt/homebrew/include
)

target_link_libraries(gx_render PUBLIC
    luna_compile_options
    glfw
    bgfx::bgfx
    spdlog::spdlog
    fmt::fmt
    Freetype::Freetype
)
