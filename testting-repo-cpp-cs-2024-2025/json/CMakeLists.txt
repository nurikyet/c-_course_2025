cmake_minimum_required(VERSION 3.16)
SET(CMAKE_INSTALL_RPATH "${PROJECT_SOURCE_DIR}/bin")
SET(TASK_NAME json)

add_compile_options(-pedantic -Wextra -fsanitize=address -fsanitize=undefined -std=c++20)

add_link_options(-pedantic -Wextra -fsanitize=address -fsanitize=undefined -std=c++20)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

enable_testing()
add_executable(${TASK_NAME}_tests tests.cpp)
add_executable(${TASK_NAME}_stress_tests stress_test.cpp)

add_test(${TASK_NAME}_tests ${Testing_SOURCE_DIR}/bin/testing)

target_link_libraries(${TASK_NAME}_tests Threads::Threads ${GTEST_LIBRARIES} ${GMOCK_BOTH_LIBRARIES})
target_link_libraries(${TASK_NAME}_stress_tests Threads::Threads ${GTEST_LIBRARIES} ${GMOCK_BOTH_LIBRARIES})

install (
        TARGETS ${TASK_NAME}_tests
        RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bin
)
