add_executable(map_editor
    main.cpp
)

target_include_directories(map_editor PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include/external
)

target_link_libraries(map_editor PRIVATE
    glfw
    bgfx::bgfx
    glm::glm
    spdlog::spdlog
    imgui::imgui
)
