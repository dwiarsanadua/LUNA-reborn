add_executable(test_combat main.cpp)
target_link_libraries(test_combat PRIVATE
    game_core
    spdlog::spdlog
    fmt::fmt
    EnTT::EnTT
    glm::glm
    SQLite3::SQLite3
)
target_include_directories(test_combat PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/game
)
target_compile_definitions(test_combat PRIVATE
    GAME_DATA_PATH="${CMAKE_SOURCE_DIR}/assets/data/game_data.db"
)
