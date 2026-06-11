# Ported: ODBC → SQLite/libpq
add_library(dbthread STATIC
    SQLiteDatabase.cpp
    DatabaseFactory.cpp
)

target_include_directories(dbthread PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(dbthread PUBLIC ${CMAKE_SOURCE_DIR}/include/external)
target_link_libraries(dbthread PUBLIC luna_compile_options)
target_link_libraries(dbthread PUBLIC SQLite3::SQLite3)
target_link_libraries(dbthread PUBLIC spdlog::spdlog)

find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(MINIAUDIO QUIET miniaudio)
endif()

# PostgreSQL (libpq) — optional build
option(WITH_POSTGRESQL "Build with PostgreSQL support" OFF)
if(WITH_POSTGRESQL)
    find_package(PostgreSQL REQUIRED)
    target_sources(dbthread PRIVATE PostgresDatabase.cpp)
    target_link_libraries(dbthread PRIVATE PostgreSQL::PostgreSQL)
    target_compile_definitions(dbthread PRIVATE HAS_POSTGRESQL)
    message(STATUS "PostgreSQL support enabled: ${PostgreSQL_LIBRARIES}")
else()
    message(STATUS "PostgreSQL support disabled (use -DWITH_POSTGRESQL=ON to enable)")
endif()
