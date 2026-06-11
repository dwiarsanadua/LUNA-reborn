# Ported: MSS → miniaudio
add_library(soundlib STATIC
    SoundLib.cpp
)

target_include_directories(soundlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(soundlib PUBLIC ${CMAKE_SOURCE_DIR}/include/external)
target_link_libraries(soundlib PUBLIC luna_compile_options)
target_link_libraries(soundlib PUBLIC spdlog::spdlog)

find_package(PkgConfig REQUIRED)
pkg_check_modules(miniaudio REQUIRED miniaudio)
target_include_directories(soundlib PUBLIC ${miniaudio_INCLUDE_DIRS})
target_link_directories(soundlib PUBLIC ${miniaudio_LIBRARY_DIRS})
target_link_libraries(soundlib PUBLIC ${miniaudio_LIBRARIES})
target_compile_options(soundlib PUBLIC ${miniaudio_CFLAGS_OTHER})
