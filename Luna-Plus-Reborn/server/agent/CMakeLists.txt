add_executable(AgentServer
    main.cpp
    AgentServer.cpp
)

target_include_directories(AgentServer PRIVATE
    ${CMAKE_SOURCE_DIR}
)

target_link_libraries(AgentServer PRIVATE
    gx_network
    Database
    spdlog::spdlog
    fmt::fmt
)
