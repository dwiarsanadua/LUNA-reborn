add_executable(server_monitor
    main.cpp
)

target_link_libraries(server_monitor PRIVATE
    spdlog::spdlog
)
