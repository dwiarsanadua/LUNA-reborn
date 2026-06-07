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
)

target_link_libraries(MapServer PRIVATE
    gx_network
    gx_physics
    Database
    EnTT::EnTT
    spdlog::spdlog
    fmt::fmt
    glm::glm
)

if(APPLE)
    target_link_libraries(MapServer PRIVATE ${APPLE_FRAMEWORKS})
endif()
