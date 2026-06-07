add_executable(DistributeServer main.cpp)
target_include_directories(DistributeServer PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/game
    ${CMAKE_BINARY_DIR}/game/fbs
)
target_link_libraries(DistributeServer PRIVATE
    game_core spdlog::spdlog fmt::fmt
)
if(APPLE)
    target_link_libraries(DistributeServer PRIVATE ${APPLE_FRAMEWORKS})
endif()
