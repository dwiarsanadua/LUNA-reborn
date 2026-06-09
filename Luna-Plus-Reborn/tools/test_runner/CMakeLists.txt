add_executable(test_runner
    main.cpp
    formulas.cpp
    systems.cpp
    network.cpp
    database.cpp
    performance.cpp
    visual.cpp
)
target_link_libraries(test_runner PRIVATE
    game_core
    spdlog::spdlog
    fmt::fmt
    EnTT::EnTT
    glm::glm
    flatbuffers::flatbuffers
    SQLite3::SQLite3
)
target_include_directories(test_runner PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/game
    ${CMAKE_SOURCE_DIR}/client
    ${CMAKE_BINARY_DIR}/game/fbs
)
target_compile_definitions(test_runner PRIVATE
    GAME_DATA_PATH="${CMAKE_SOURCE_DIR}/assets/data/game_data.db"
)
include(CTest)
add_test(NAME test_runner COMMAND test_runner WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
set_tests_properties(test_runner PROPERTIES
    TIMEOUT 60
    ENVIRONMENT "SPDLOG_LEVEL=info"
)
