set(COMMUNICATION_SOURCES
  server.cpp
  tcp_player.cpp
  message.cpp
)

set(COMMUNICATION_HEADERS
  server.h
  tcp_player.h
  message.h
)

add_library(communication ${COMMUNICATION_SOURCES})

add_style_check_target(communication "${COMMUNICATION_SOURCES}" SnakeServer)
add_style_check_target(communication "${COMMUNICATION_HEADERS}" SnakeServer)
