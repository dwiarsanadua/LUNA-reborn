add_library(Database STATIC
    Database.cpp
    BcryptUtils.cpp
    RateLimiter.cpp
    ValidationSystem.cpp
)

target_include_directories(Database PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_include_directories(Database PRIVATE
    ${CMAKE_SOURCE_DIR}/game
)

target_link_libraries(Database PRIVATE
    SQLite3::SQLite3
    spdlog::spdlog
    fmt::fmt
)
