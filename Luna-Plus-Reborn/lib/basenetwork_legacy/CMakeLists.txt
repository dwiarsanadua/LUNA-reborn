# Ported: IOCP → libuv
# Source files will be rewritten in Phase 4
add_library(basenetwork STATIC
    NetworkThread.cpp
    TcpListener.cpp
    TcpConnection.cpp
    PacketBuffer.cpp
    SessionManager.cpp
)

target_include_directories(basenetwork PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(basenetwork PUBLIC luna_compile_options)
target_link_libraries(basenetwork PUBLIC libuv::uv)
