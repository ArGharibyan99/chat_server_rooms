file(GLOB TEST_SOURCES "*.cpp")
add_executable(tests ${TEST_SOURCES})
target_link_libraries(tests PRIVATE MyProjectLib)
add_test(NAME MyProjectTests COMMAND tests)

