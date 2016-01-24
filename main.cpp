#include <cstdlib>
#include <memory>

#include "boost/asio.hpp"

#include "communication/server.h"
#include "communication/game_logger.h"
#include "games/lobby.h"
#include "games/snake/match.h"
#include "gflags/gflags.h"

DEFINE_int32(port, 8091, "Port to start a server on.");

int main(int argc, char** argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  try {
    printf("Starting server on port %d.\n", FLAGS_port);

    boost::asio::io_service io_service;

    unique_ptr<RedisClient> redis_client(new RedisClient());
    unique_ptr<GameLoggerFactory> game_logger_factory(
        new RedisGameLoggerFactory(redis_client.get()));
    unique_ptr<MatchFactory> match_factory(new SnakeMatchFactory());
    unique_ptr<Lobby> lobby(new SequentialLobby(std::move(match_factory),
                                                std::move(game_logger_factory)));

    Server s(&io_service, FLAGS_port, std::move(lobby));
    io_service.run();
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
