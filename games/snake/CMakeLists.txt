set(SNAKE_SOURCES
  match.cpp
)

set(SNAKE_HEADERS
  match.h
)

add_library(snake ${SNAKE_SOURCES})

add_style_check_target(snake "${SNAKE_SOURCES}" SnakeServer)
add_style_check_target(snake "${SNAKE_HEADERS}" SnakeServer)
