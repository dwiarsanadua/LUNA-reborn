add_executable(test_login main.cpp)
target_link_libraries(test_login PRIVATE game_core spdlog::spdlog fmt::fmt)
target_include_directories(test_login PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/game)
target_compile_definitions(test_login PRIVATE
    GAME_DATA_PATH="${CMAKE_SOURCE_DIR}/assets/data/game_data.db"
)
