find_package(imgui QUIET)

if(imgui_FOUND)
    add_executable(map_editor
        main.cpp
        imgui_impl_bgfx.cpp
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
else()
    message(WARNING "imgui not found — skipping map_editor build")
endif()
