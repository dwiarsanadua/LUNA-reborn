add_library(gx_scripting STATIC
    LuaEngine.cpp
)

target_include_directories(gx_scripting PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(gx_scripting PRIVATE
    sol2::sol2
    spdlog::spdlog
    fmt::fmt
)

find_library(LUA_LIBRARY NAMES lua lua5.4 lua5.5 PATHS /opt/homebrew/lib /usr/local/lib)
find_path(LUA_INCLUDE_DIR lua.h PATHS /opt/homebrew/include /opt/homebrew/include/lua /usr/local/include)
if(LUA_LIBRARY AND LUA_INCLUDE_DIR)
    target_include_directories(gx_scripting PRIVATE ${LUA_INCLUDE_DIR})
    target_link_libraries(gx_scripting PUBLIC ${LUA_LIBRARY})
    target_compile_definitions(gx_scripting PUBLIC LUNA_HAS_LUA=1)
else()
    message(WARNING "Lua library not found; MapScriptRuntime will be stubbed")
endif()
