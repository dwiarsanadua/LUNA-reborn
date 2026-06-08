add_executable(MapServer
    main.cpp
    MapServer.cpp
    systems/CombatSystem.cpp
    systems/AISystem.cpp
    systems/MovementSystem.cpp
    systems/ItemSystem.cpp
    systems/QuestSystem.cpp
)

target_include_directories(MapServer PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/game
    ${CMAKE_BINARY_DIR}/game/fbs
)

target_link_libraries(MapServer PRIVATE
    gx_network
    gx_physics
    gx_geom
    Database
    game_core
    EnTT::EnTT
    spdlog::spdlog
    fmt::fmt
    glm::glm
)

if(APPLE)
    # Frameworks are linked via bgfx::bgfx and Jolt::Jolt INTERFACE_LINK_OPTIONS
endif()
