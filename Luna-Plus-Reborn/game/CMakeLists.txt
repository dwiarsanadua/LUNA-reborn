add_library(game_core STATIC
    ecs/components/Inventory.cpp
    ecs/components/Equipment.cpp
    ecs/components/QuestLog.cpp
    ecs/systems/MovementSystem.cpp
    ecs/systems/CombatSystem.cpp
    ecs/systems/AISystem.cpp
    ecs/systems/SpawnSystem.cpp
    ecs/systems/QuestSystem.cpp
    ecs/systems/SkillSystem.cpp
    ecs/systems/ItemSystem.cpp
    ecs/systems/LootSystem.cpp
    ecs/systems/PartySystem.cpp
    ecs/systems/ShopSystem.cpp
    ecs/systems/HousingSystem.cpp
    ecs/systems/PetSystem.cpp
    ecs/systems/FishingSystem.cpp
    ecs/systems/GuildSystem.cpp
    ecs/systems/VehicleSystem.cpp
    ecs/systems/CookingSystem.cpp
    ecs/systems/ParticleSystem.cpp
    ecs/systems/AnimationSystem.cpp
    ecs/systems/GameDataDB.cpp
    ecs/systems/FSMEngine.cpp
    ecs/systems/WeatherSystem.cpp
    ecs/systems/ComboSystem.cpp
    network/TcpServer.cpp
    network/LoginHandler.cpp
    network/CharacterDB.cpp
)

target_include_directories(game_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/ecs
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/data
)

target_link_libraries(game_core PUBLIC
    EnTT::EnTT
    flatbuffers::flatbuffers
    SQLite3::SQLite3
    spdlog::spdlog
    fmt::fmt
    glm::glm
    libuv::uv
)

target_link_libraries(game_core PRIVATE
    gx_geom
    gx_physics
)

file(GLOB FBS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/network/protocol/*.fbs)
if(FBS_FILES)
    set(FBS_GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/fbs)
    set(FBS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/network/protocol)
    foreach(FBS ${FBS_FILES})
        get_filename_component(FBS_NAME ${FBS} NAME_WE)
        add_custom_command(
            OUTPUT ${FBS_GENERATED_DIR}/${FBS_NAME}_generated.h
            COMMAND flatbuffers::flatc --cpp -I ${FBS_INCLUDE_DIR} -o ${FBS_GENERATED_DIR} ${FBS}
            DEPENDS ${FBS}
            COMMENT "Generating flatbuffers: ${FBS_NAME}"
        )
        target_sources(game_core PRIVATE ${FBS_GENERATED_DIR}/${FBS_NAME}_generated.h)
    endforeach()
    target_include_directories(game_core PRIVATE ${FBS_GENERATED_DIR})
endif()

target_compile_definitions(game_core PRIVATE
    GAME_DATA_PATH="${CMAKE_SOURCE_DIR}/assets/data/game_data.db"
    ASSETS_PATH="${CMAKE_SOURCE_DIR}/assets/"
)
