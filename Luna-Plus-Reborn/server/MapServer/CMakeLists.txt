add_executable(MapServer
    main.cpp
    systems/MapECSWorld.cpp
    DungeonManager.cpp
)
target_include_directories(MapServer PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/game
    ${CMAKE_BINARY_DIR}/game/fbs
)
target_compile_definitions(MapServer PRIVATE
    GAME_DATA_PATH="${CMAKE_SOURCE_DIR}/assets/data/game_data.db"
)
target_link_libraries(MapServer PRIVATE
    game_core spdlog::spdlog fmt::fmt
)
if(APPLE)
    target_link_libraries(MapServer PRIVATE ${APPLE_FRAMEWORKS})
endif()
