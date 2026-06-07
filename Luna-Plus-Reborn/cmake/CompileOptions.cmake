if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang")
    add_compile_options(
        -Wall -Wextra -Wpedantic
        -Wno-c++98-compat-pedantic
        -Wno-float-equal
        -Wno-missing-prototypes
        -Wno-unused-parameter
        -Wno-unused-variable
        -Wno-unused-but-set-variable
        -fPIC
        -fvisibility=hidden
    )

    if(APPLE)
        add_compile_options(-mmacosx-version-min=14.0 -stdlib=libc++)
    endif()

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g -O0)
    else()
        add_compile_options(-O2 -DNDEBUG)
    endif()
endif()

add_library(luna_compile_options INTERFACE)
target_compile_definitions(luna_compile_options INTERFACE
    $<$<CONFIG:Debug>:DEBUG _DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
    LUNAPLUS_VERSION="${PROJECT_VERSION}"
)
