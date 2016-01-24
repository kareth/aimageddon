set(GAMES_SOURCES
  lobby.cpp
  match.cpp
)

set(GAMES_HEADERS
  lobby.h
  match.h
)

set(GAMES_TESTS
  lobby_test.cpp
)

add_subdirectory(snake)
add_subdirectory(round_based)

add_library(games ${GAMES_SOURCES})
target_link_libraries(games snake round_based gflags)

add_style_check_target(games "${GAMES_SOURCES}" SnakeServer)
add_style_check_target(games "${GAMES_HEADERS}" SnakeServer)

add_executable(games_tests ${GAMES_TESTS})
target_link_libraries(games_tests games communication gmock gtest gtest_main cpp_redis)
add_test(games_tests games_tests)
