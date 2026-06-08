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
    ${CMAKE_SOURCE_DIR}/game
    ${CMAKE_BINARY_DIR}/game/fbs
)
target_link_libraries(test_startup PRIVATE
    gx_network
    Database
    game_core
    spdlog::spdlog
    fmt::fmt
    flatbuffers::flatbuffers
)

target_compile_definitions(test_startup PRIVATE
    LoginResult_LOGIN_OK=LoginResult_Success
    LoginResult_LOGIN_FAILED=LoginResult_InvalidCredentials
)
