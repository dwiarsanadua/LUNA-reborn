add_library(gx_network STATIC
    NetworkLayer.cpp
)

target_include_directories(gx_network PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(gx_network PRIVATE
    asio::asio
    spdlog::spdlog
    fmt::fmt
)
