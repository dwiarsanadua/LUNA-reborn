add_executable(MapServer
    main.cpp
    MapServer.cpp
    systems/CombatSystem.cpp
    systems/AISystem.cpp
    systems/MovementSystem.cpp
    systems/ItemSystem.cpp
    systems/QuestSystem.cpp
    systems/StallSystem.cpp
    systems/GridSystem.cpp
    systems/TriggerSystem.cpp
    systems/MapScriptRuntime.cpp
    systems/SecondarySystem.cpp
)

target_include_directories(MapServer PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/game
    ${CMAKE_SOURCE_DIR}/engine
    ${CMAKE_BINARY_DIR}/game/fbs
)

target_compile_definitions(MapServer PRIVATE
    GAME_DATA_PATH="${CMAKE_SOURCE_DIR}/assets/data/game_data.db"
)

target_link_libraries(MapServer PRIVATE
    gx_network
    gx_physics
    gx_geom
    gx_scripting
    Database
    game_core
    EnTT::EnTT
    flatbuffers::flatbuffers
    spdlog::spdlog
    fmt::fmt
    glm::glm
)

if(APPLE)
    # Frameworks are linked via bgfx::bgfx and Jolt::Jolt INTERFACE_LINK_OPTIONS
endif()
