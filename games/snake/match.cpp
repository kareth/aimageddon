#include "games/snake/match.h"

#include <unistd.h>

SnakeMatchFactory::SnakeMatchFactory() {
}

unique_ptr<Match> SnakeMatchFactory::CreateMatch(
    const Json& match_params) {
  SnakeMatch::Options opts;
  if (opts.ParseFromJson(match_params))
    return nullptr;

  return unique_ptr<Match>(new SnakeMatch(opts));
}

int SnakeMatch::Options::ParseFromJson(const Json& json) {
  num_players = json.get("num_players", Json(0)).as_int();
  if (num_players < 1 || num_players > 4) return 1;
  return 0;
}

bool SnakeMatch::Options::IsCompatible(const Options& rhs) {
  return num_players == rhs.num_players;
}

namespace {

Message MakeMessage(const string& type) {
  return Message("{\"type\":\"" + type + "\"}");
}

}  // namespace

SnakeMatch::SnakeMatch(const Options& options)
  : Match(options.num_players), options_(options) {
}

bool SnakeMatch::CheckOptionsCompatibility(const Json& match_params) {
  SnakeMatch::Options opts;
  if (opts.ParseFromJson(match_params)) return false;

  return options_.IsCompatible(opts);
}

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
