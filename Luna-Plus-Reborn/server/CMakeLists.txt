# ─── Servers ─────────────────────────────────────────────
add_subdirectory(shared)
add_subdirectory(agent)
add_subdirectory(distribute)
add_subdirectory(map)

# ─── Integration Tests ────────────────────────────────────
add_executable(test_startup
    test_startup.cpp
    agent/AgentServer.cpp
)
target_include_directories(test_startup PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/game
    ${CMAKE_BINARY_DIR}/game/fbs
)
target_link_libraries(test_startup PRIVATE
    gx_network
    Database
    game_core
    spdlog::spdlog
    fmt::fmt
    flatbuffers::flatbuffers
)

target_compile_definitions(test_startup PRIVATE
    LoginResult_LOGIN_OK=LoginResult_Success
    LoginResult_LOGIN_FAILED=LoginResult_InvalidCredentials
)

add_executable(test_phase5
    test_phase5.cpp
    map/MapServer.cpp
    map/systems/CombatSystem.cpp
    map/systems/AISystem.cpp
    map/systems/MovementSystem.cpp
    map/systems/ItemSystem.cpp
    map/systems/QuestSystem.cpp
    map/systems/StallSystem.cpp
    map/systems/GridSystem.cpp
    map/systems/TriggerSystem.cpp
    map/systems/MapScriptRuntime.cpp
    map/systems/SecondarySystem.cpp
)
target_include_directories(test_phase5 PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/game
    ${CMAKE_SOURCE_DIR}/engine
    ${CMAKE_BINARY_DIR}/game/fbs
)
target_link_libraries(test_phase5 PRIVATE
    gx_network
    gx_physics
    gx_geom
    gx_scripting
    Database
    game_core
    EnTT::EnTT
    spdlog::spdlog
    fmt::fmt
    flatbuffers::flatbuffers
    glm::glm
)
target_compile_definitions(test_phase5 PRIVATE
    GAME_DATA_PATH="${CMAKE_SOURCE_DIR}/assets/data/game_data.db"
)

add_executable(test_phase6
    test_phase6.cpp
    map/MapServer.cpp
    map/systems/CombatSystem.cpp
    map/systems/AISystem.cpp
    map/systems/MovementSystem.cpp
    map/systems/ItemSystem.cpp
    map/systems/QuestSystem.cpp
    map/systems/StallSystem.cpp
    map/systems/GridSystem.cpp
    map/systems/TriggerSystem.cpp
    map/systems/MapScriptRuntime.cpp
    map/systems/SecondarySystem.cpp
)
target_include_directories(test_phase6 PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/game
    ${CMAKE_SOURCE_DIR}/engine
    ${CMAKE_BINARY_DIR}/game/fbs
)
target_link_libraries(test_phase6 PRIVATE
    gx_network
    gx_physics
    gx_geom
    gx_scripting
    Database
    game_core
    EnTT::EnTT
    spdlog::spdlog
    fmt::fmt
    flatbuffers::flatbuffers
    glm::glm
)
target_compile_definitions(test_phase6 PRIVATE
    GAME_DATA_PATH="${CMAKE_SOURCE_DIR}/assets/data/game_data.db"
)
