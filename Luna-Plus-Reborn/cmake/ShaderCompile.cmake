# Shader compilation helper for LUNA Plus Reborn
# Usage: compile_shaders(TARGET_NAME SOURCES...)

function(compile_shaders TARGET_NAME)
    set(SHADERC_PATH "")
    
    # Hardcoded path for the user's machine to ensure compilation works
    set(SHADERC_EXE "/Users/macbookair/PRIBADI/luna-plus-master/external/bgfx/tools/bin/darwin/shaderc")
    
    if(NOT EXISTS "${SHADERC_EXE}")
        # Try to find shaderc fallback
        if(APPLE)
            find_program(SHADERC_EXE shaderc PATHS 
                /opt/homebrew/bin 
                /usr/local/bin
                /Users/macbookair/PRIBADI/luna-plus-master/external/bgfx/tools/bin/darwin
            )
        elseif(WIN32)
            find_program(SHADERC_EXE shaderc PATHS ${VCPKG_INSTALLATION_ROOT}/downloads/tools/bgfx)
        endif()
    endif()
    
    set(OUTPUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders")

    if(NOT SHADERC_EXE)
        message(WARNING "shaderc not found! Copying existing .bin shaders as fallback.")
        file(GLOB EXISTING_BINS "${CMAKE_SOURCE_DIR}/shaders/*.bin")
        file(COPY ${EXISTING_BINS} DESTINATION "${OUTPUT_DIR}")
        return()
    endif()

    set(VARYING_DEF "${CMAKE_SOURCE_DIR}/shaders/varying.def.sc")
    file(MAKE_DIRECTORY "${OUTPUT_DIR}")

    foreach(SHADER_SOURCE ${ARGN})
        get_filename_component(SHADER_NAME ${SHADER_SOURCE} NAME_WE)
        get_filename_component(SHADER_EXT ${SHADER_SOURCE} EXT)
        
        set(PROFILE "")
        set(TYPE "")
        
        if(SHADER_NAME MATCHES "^vs_")
            set(TYPE "vertex")
            if(APPLE)
                set(PROFILE "metal")
            elseif(WIN32)
                set(PROFILE "s_5_0")
            else()
                set(PROFILE "120")
            endif()
        elseif(SHADER_NAME MATCHES "^fs_")
            set(TYPE "fragment")
            if(APPLE)
                set(PROFILE "metal")
            elseif(WIN32)
                set(PROFILE "s_5_0")
            else()
                set(PROFILE "120")
            endif()
        endif()

        if(NOT TYPE STREQUAL "")
            set(OUTPUT_FILE "${OUTPUT_DIR}/${SHADER_NAME}.bin")
            
            set(BGFX_SRC_INCLUDE "/Users/macbookair/PRIBADI/luna-plus-master/external/bgfx/src")
            set(PLATFORM "osx")
            if(WIN32)
                set(PLATFORM "windows")
            endif()
            
            add_custom_command(
                OUTPUT "${OUTPUT_FILE}"
                COMMAND ${SHADERC_EXE}
                ARGS -f "${SHADER_SOURCE}" -o "${OUTPUT_FILE}" -i "${CMAKE_SOURCE_DIR}/shaders" -i "${BGFX_SRC_INCLUDE}" --varyingdef "${VARYING_DEF}" --type ${TYPE} --platform ${PLATFORM} -p ${PROFILE}
                DEPENDS "${SHADER_SOURCE}" "${VARYING_DEF}"
                COMMENT "Compiling shader ${SHADER_NAME} for ${PROFILE}..."
            )
            list(APPEND SHADER_BINARIES "${OUTPUT_FILE}")
        endif()
    endforeach()

    add_custom_target(${TARGET_NAME} ALL DEPENDS ${SHADER_BINARIES})
endfunction()
