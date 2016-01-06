set(SNAKE_SOURCES
  match.cpp
)

set(SNAKE_HEADERS
  match.h
)

set(SNAKE_TESTS
  match_test.cpp
)

add_library(snake ${SNAKE_SOURCES})

add_style_check_target(snake "${SNAKE_SOURCES}" SnakeServer)
add_style_check_target(snake "${SNAKE_HEADERS}" SnakeServer)

add_executable(snake_tests ${SNAKE_TESTS})
target_link_libraries(snake_tests snake communication gmock gtest gtest_main)
add_test(snake_tests snake_tests)
