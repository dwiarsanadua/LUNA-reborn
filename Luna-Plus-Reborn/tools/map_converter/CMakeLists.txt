add_executable(map_converter
    main.cpp
)
target_link_libraries(map_converter PRIVATE
    glm::glm
)
set_target_properties(map_converter PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tools/map_converter"
)
