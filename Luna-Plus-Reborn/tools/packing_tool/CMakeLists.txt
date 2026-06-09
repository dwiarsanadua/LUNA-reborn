add_executable(packing_tool main.cpp)
target_link_libraries(packing_tool PRIVATE
    spdlog::spdlog
    fmt::fmt
)
