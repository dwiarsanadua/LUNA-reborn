add_library(ziparchive STATIC
    ZipArchive.cpp
    ZipAutoBuffer.cpp
    ZipCentralDir.cpp
    ZipCompatibility.cpp
    ZipException.cpp
    ZipFile.cpp
    ZipFileHeader.cpp
    ZipMemFile.cpp
    ZipPathComponent.cpp
    ZipPlatform.cpp
    ZipPlatformComm.cpp
    ZipStorage.cpp
    ZipString.cpp
    stdafx.cpp
)

target_include_directories(ziparchive PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/zlib
    ${CMAKE_SOURCE_DIR}/include/external
)
target_link_libraries(ziparchive PUBLIC luna_compile_options)
