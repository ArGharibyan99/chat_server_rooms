file(GLOB TEST_SOURCES "*.cpp")
add_executable(tests ${TEST_SOURCES})
target_link_libraries(tests PRIVATE ChatRoomLib)
add_test(NAME ChatRoomTests COMMAND tests)

