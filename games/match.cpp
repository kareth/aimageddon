#include "games/match.h"

Match::Match(int expected_players)
  : num_players_(expected_players) {
}

void Match::AddPlayer(unique_ptr<Player> player) {
  players_.push_back(std::move(player));
}

void Match::Broadcast(const Message& message) {
  for (auto& player : players_) {
    player->Write(message);
  }
}

void Match::Publish(const Message& message) {
  // TODO(pzurkowski) Unimplemented
  exit(1);
}
