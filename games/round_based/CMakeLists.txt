set(ROUND_BASED_SOURCES
  match.cpp
  player.cpp
)

set(ROUND_BASED_HEADERS
  match.h
  player.h
)

add_library(round_based ${ROUND_BASED_SOURCES})

add_style_check_target(round_based "${ROUND_BASED_SOURCES}" SnakeServer)
add_style_check_target(round_based "${ROUND_BASED_HEADERS}" SnakeServer)
