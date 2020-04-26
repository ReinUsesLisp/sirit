add_executable(${PROJECT_NAME}_tests
    main.cpp)
target_link_libraries(${PROJECT_NAME}_tests PRIVATE sirit)
add_test(TESTS ${PROJECT_NAME}_tests)
