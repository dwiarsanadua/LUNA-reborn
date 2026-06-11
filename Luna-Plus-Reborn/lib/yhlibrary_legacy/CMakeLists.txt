add_library(yhlibrary STATIC
    Encryptor.cpp
    Fileio.cpp
    MemoryPool.cpp
    MemoryPoolTempl.cpp
    PtrList.cpp
    StdAfx.cpp
    StrTokenizer.cpp
    Strclass.cpp
    cConstLinkedList.cpp
    cLinkedList.cpp
    cLooseLinkedList.cpp
    IndexGenerator.cpp
    connection.cpp
    hashtable.cpp
)

target_include_directories(yhlibrary PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(yhlibrary BEFORE PUBLIC ${CMAKE_SOURCE_DIR}/include/external)
target_include_directories(yhlibrary PUBLIC ${CMAKE_SOURCE_DIR}/include/external)
target_link_libraries(yhlibrary PUBLIC luna_compile_options)
