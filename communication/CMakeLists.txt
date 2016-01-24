set(COMMUNICATION_SOURCES
  server.cpp
  game_logger.cpp
  tcp_connection.cpp
  message.cpp
)

set(COMMUNICATION_HEADERS
  server.h
  game_logger.h
  connection.h
  tcp_connection.h
  message.h
)

add_library(communication ${COMMUNICATION_SOURCES})

add_style_check_target(communication "${COMMUNICATION_SOURCES}" SnakeServer)
add_style_check_target(communication "${COMMUNICATION_HEADERS}" SnakeServer)
