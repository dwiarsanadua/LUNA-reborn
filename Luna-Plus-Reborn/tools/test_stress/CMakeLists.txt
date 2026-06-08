add_executable(test_stress main.cpp)
target_include_directories(test_stress PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/game
    ${CMAKE_BINARY_DIR}/game/fbs
)
target_link_libraries(test_stress PRIVATE
    flatbuffers::flatbuffers
)
add_dependencies(test_stress game_core)
