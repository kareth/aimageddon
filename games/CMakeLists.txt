set(GAMES_SOURCES
  lobby.cpp
  match.cpp
)

set(GAMES_HEADERS
  lobby.h
  match.h
)

add_subdirectory(snake)

add_library(games ${GAMES_SOURCES})
target_link_libraries(games snake)

add_style_check_target(games "${GAMES_SOURCES}" SnakeServer)
add_style_check_target(games "${GAMES_HEADERS}" SnakeServer)
