# Shader compilation helper for LUNA Plus Reborn
# Usage: compile_shaders(TARGET_NAME SOURCES...)

function(compile_shaders TARGET_NAME)
    set(BGFX_ROOT "${CMAKE_SOURCE_DIR}/external/bgfx")
    if(NOT EXISTS "${BGFX_ROOT}/src")
        set(BGFX_ROOT "${CMAKE_SOURCE_DIR}/../external/bgfx")
    endif()

    if(APPLE)
        set(SHADERC_PLATFORM_DIRS
            "${BGFX_ROOT}/tools/bin/darwin"
            "${BGFX_ROOT}/.build/osx-arm64/bin")
    elseif(WIN32)
        set(SHADERC_PLATFORM_DIRS
            "${BGFX_ROOT}/tools/bin/windows"
            "${BGFX_ROOT}/.build/win64_vs2022/bin")
    else()
        set(SHADERC_PLATFORM_DIRS
            "${BGFX_ROOT}/tools/bin/linux"
            "${BGFX_ROOT}/.build/linux64_gcc/bin")
    endif()

    set(SHADERC_EXE "")
    foreach(DIR ${SHADERC_PLATFORM_DIRS})
        foreach(NAME shaderc shaderc.exe shadercRelease)
            if(NOT SHADERC_EXE AND EXISTS "${DIR}/${NAME}")
                set(SHADERC_EXE "${DIR}/${NAME}")
            endif()
        endforeach()
    endforeach()
    if(NOT SHADERC_EXE AND EXISTS "${BGFX_ROOT}/.build/ci/tools/bin/shaderc")
        set(SHADERC_EXE "${BGFX_ROOT}/.build/ci/tools/bin/shaderc")
    endif()
    if(NOT SHADERC_EXE)
        find_program(SHADERC_EXE shaderc
            PATHS
                ${SHADERC_PLATFORM_DIRS}
                "${BGFX_ROOT}/.build/ci/tools/bin"
                /opt/homebrew/bin
                /usr/local/bin
        )
    endif()

    set(OUTPUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders")
    set(BGFX_SRC_INCLUDE "${BGFX_ROOT}/src")

    if(NOT SHADERC_EXE OR NOT EXISTS "${SHADERC_EXE}")
        message(WARNING "shaderc not found under ${BGFX_ROOT}. Copying existing .bin shaders as fallback.")
        file(GLOB EXISTING_BINS "${CMAKE_SOURCE_DIR}/shaders/*.bin")
        if(EXISTING_BINS)
            file(COPY ${EXISTING_BINS} DESTINATION "${OUTPUT_DIR}")
        endif()
        return()
    endif()

    message(STATUS "ShaderCompile: using shaderc at ${SHADERC_EXE}")

    set(VARYING_DEF "${CMAKE_SOURCE_DIR}/shaders/varying.def.sc")
    file(MAKE_DIRECTORY "${OUTPUT_DIR}")

    foreach(SHADER_SOURCE ${ARGN})
        get_filename_component(SHADER_NAME ${SHADER_SOURCE} NAME_WE)

        set(TYPE "")
        set(PROFILE "")
        set(PLATFORM "osx")

        if(SHADER_NAME MATCHES "^vs_")
            set(TYPE "vertex")
        elseif(SHADER_NAME MATCHES "^fs_")
            set(TYPE "fragment")
        endif()

        if(APPLE)
            set(PROFILE "metal")
            set(PLATFORM "osx")
        elseif(WIN32)
            set(PROFILE "s_5_0")
            set(PLATFORM "windows")
        else()
            set(PROFILE "spirv")
            set(PLATFORM "linux")
        endif()

        if(NOT TYPE STREQUAL "")
            set(OUTPUT_FILE "${OUTPUT_DIR}/${SHADER_NAME}.bin")

            add_custom_command(
                OUTPUT "${OUTPUT_FILE}"
                COMMAND ${SHADERC_EXE}
                ARGS -f "${SHADER_SOURCE}" -o "${OUTPUT_FILE}"
                     -i "${CMAKE_SOURCE_DIR}/shaders"
                     -i "${BGFX_SRC_INCLUDE}"
                     --varyingdef "${VARYING_DEF}"
                     --type ${TYPE}
                     --platform ${PLATFORM}
                     -p ${PROFILE}
                DEPENDS "${SHADER_SOURCE}" "${VARYING_DEF}"
                COMMENT "Compiling shader ${SHADER_NAME} (${PROFILE})"
            )
            list(APPEND SHADER_BINARIES "${OUTPUT_FILE}")
        endif()
    endforeach()

    if(SHADER_BINARIES)
        add_custom_target(${TARGET_NAME} ALL DEPENDS ${SHADER_BINARIES})
    endif()
endfunction()
