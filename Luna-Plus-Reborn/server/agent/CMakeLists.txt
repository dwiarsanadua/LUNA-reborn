add_executable(AgentServer
    main.cpp
    AgentServer.cpp
)

target_include_directories(AgentServer PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/game
    ${CMAKE_BINARY_DIR}/game/fbs
)

target_link_libraries(AgentServer PRIVATE
    gx_network
    Database
    game_core
    spdlog::spdlog
    fmt::fmt
    flatbuffers::flatbuffers
)

target_compile_definitions(AgentServer PRIVATE
    LoginResult_LOGIN_OK=LoginResult_Success
    LoginResult_LOGIN_FAILED=LoginResult_InvalidCredentials
)
