add_library(Database STATIC
    Database.cpp
)

target_include_directories(Database PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Database PRIVATE
    SQLite3::SQLite3
    spdlog::spdlog
    fmt::fmt
)
