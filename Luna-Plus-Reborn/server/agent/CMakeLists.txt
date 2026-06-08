add_executable(AgentServer
    main.cpp
    AgentServer.cpp
)

target_include_directories(AgentServer PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}/game/fbs
)

target_link_libraries(AgentServer PRIVATE
    gx_network
    Database
    game_core
    spdlog::spdlog
    fmt::fmt
)
