# ─── Servers ─────────────────────────────────────────────
add_subdirectory(shared)
add_subdirectory(agent)
add_subdirectory(distribute)
add_subdirectory(map)

# ─── Integration Tests ────────────────────────────────────
add_executable(test_startup
    test_startup.cpp
    agent/AgentServer.cpp
)
target_include_directories(test_startup PRIVATE
    ${CMAKE_SOURCE_DIR}
)
target_link_libraries(test_startup PRIVATE
    gx_network
    Database
    spdlog::spdlog
    fmt::fmt
)
