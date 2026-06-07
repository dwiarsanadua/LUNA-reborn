add_library(gx_resource STATIC
    ResourceCache.cpp
)

target_include_directories(gx_resource PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include/external
)

target_link_libraries(gx_resource PUBLIC
    luna_compile_options
)
