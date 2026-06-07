add_executable(chx_to_gltf
    main.cpp
)

target_link_libraries(chx_to_gltf PRIVATE
    glm::glm
)

# Tools run from project root, so set working directory
set_target_properties(chx_to_gltf PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tools/chx_to_gltf"
)
