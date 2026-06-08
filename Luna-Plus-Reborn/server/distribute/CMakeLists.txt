add_executable(DistributeServer
    main.cpp
    DistributeServer.cpp
)

target_include_directories(DistributeServer PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/game
    ${CMAKE_BINARY_DIR}/game/fbs
)

target_link_libraries(DistributeServer PRIVATE
    gx_network
    Database
    spdlog::spdlog
    fmt::fmt
    flatbuffers::flatbuffers
    EnTT::EnTT
)
