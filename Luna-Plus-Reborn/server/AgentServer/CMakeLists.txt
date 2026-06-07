add_executable(AgentServer main.cpp)
target_include_directories(AgentServer PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/game
    ${CMAKE_BINARY_DIR}/game/fbs
)
target_link_libraries(AgentServer PRIVATE
    game_core spdlog::spdlog fmt::fmt
)
if(APPLE)
    target_link_libraries(AgentServer PRIVATE ${APPLE_FRAMEWORKS})
endif()
