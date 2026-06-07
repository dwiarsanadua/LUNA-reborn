add_executable(test_fbs main.cpp)
target_include_directories(test_fbs PRIVATE ${CMAKE_BINARY_DIR}/game/fbs)
target_link_libraries(test_fbs PRIVATE flatbuffers::flatbuffers)
