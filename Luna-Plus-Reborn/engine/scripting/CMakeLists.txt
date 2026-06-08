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

# Prefer LuaJIT when available (Phase 5), fall back to system Lua.
find_library(LUAJIT_LIBRARY
    NAMES luajit-5.1 luajit
    PATHS /opt/homebrew/lib /opt/homebrew/opt/luajit/lib /usr/local/lib)
find_path(LUAJIT_INCLUDE_DIR
    NAMES luajit.h lua.h
    PATHS /opt/homebrew/include/luajit-2.1 /opt/homebrew/opt/luajit/include/luajit-2.1
          /opt/homebrew/include /usr/local/include)

if(LUAJIT_LIBRARY AND LUAJIT_INCLUDE_DIR)
    target_include_directories(gx_scripting PRIVATE ${LUAJIT_INCLUDE_DIR})
    target_link_libraries(gx_scripting PUBLIC ${LUAJIT_LIBRARY})
    target_compile_definitions(gx_scripting PUBLIC LUNA_HAS_LUA=1 LUNA_LUA_JIT=1 SOL_LUAJIT=1)
    message(STATUS "gx_scripting: using LuaJIT")
else()
    find_library(LUA_LIBRARY NAMES lua lua5.4 lua5.5 PATHS /opt/homebrew/lib /usr/local/lib)
    find_path(LUA_INCLUDE_DIR lua.h PATHS /opt/homebrew/include /opt/homebrew/include/lua /usr/local/include)
    if(LUA_LIBRARY AND LUA_INCLUDE_DIR)
        target_include_directories(gx_scripting PRIVATE ${LUA_INCLUDE_DIR})
        target_link_libraries(gx_scripting PUBLIC ${LUA_LIBRARY})
        target_compile_definitions(gx_scripting PUBLIC LUNA_HAS_LUA=1 LUNA_LUA_JIT=0)
        message(STATUS "gx_scripting: using Lua (install luajit for LuaJIT backend)")
    else()
        message(WARNING "Lua library not found; MapScriptRuntime will be stubbed")
    endif()
endif()
