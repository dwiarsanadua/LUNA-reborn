add_library(soundlib STATIC
    soundlib/SoundLib.cpp
)

target_include_directories(soundlib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include/external
)
if(APPLE)
    target_include_directories(soundlib PUBLIC /opt/homebrew/include)
endif()

target_link_libraries(soundlib PUBLIC
    spdlog::spdlog
)

if(NOT WIN32 AND NOT APPLE)
    find_package(Threads REQUIRED)
    target_link_libraries(soundlib PUBLIC Threads::Threads ${CMAKE_DL_LIBS} m)
endif()
