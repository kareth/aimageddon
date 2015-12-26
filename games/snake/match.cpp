#include "games/snake/match.h"

#include <unistd.h>

SnakeMatch::SnakeMatch(const Options& options)
  : Match(options.num_players) {
}

namespace {
Message MakeMessage(const string& type) {
  return Message("{\"type\":\"" + type + "\"}");
}
}  // namespace

void SnakeMatch::StartGame() {
  // TODO(pzurkowski) this is a temporary implementation just to test server and lobby
  Broadcast(MakeMessage("GameStart"));

  int num_turns = 10;
  while (num_turns--) {
    usleep(1000000);
    for (auto& player : players_) {
      auto msg = player->Read();
    }
    Broadcast(MakeMessage("NextTurn"));
  }
  Broadcast(MakeMessage("GameEnd"));
  // TODO(pzurkowski) return to lobby
}
