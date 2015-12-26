#include "games/lobby.h"

SimpleLobby::SimpleLobby() {}

SimpleLobby::~SimpleLobby() {
  for (auto& t : match_threads_) {
    t.second.join();
  }
}

void SimpleLobby::AddPlayer(unique_ptr<Player> p) {
  int id = ++player_id_counter_;
  waiting_players_[id] = std::move(p);
  WaitForJoin(id);
}

void SimpleLobby::JoinRequest(int player_id, unique_ptr<Message> message) {
  if (message->content().get("type") != "JoinGame") {
    WaitForJoin(player_id);
  }

  int match_id = ++match_id_counter_;
  // TODO(pzurkowski) MatchFactory?
  matches_[match_id] = unique_ptr<Match>(new SnakeMatch(SnakeMatch::Options{1}));

  // TODO(pzurkowski) Add a callback with game end to remove finished matches and/or
  // recycle players back into the lobby.
  match_threads_[match_id] =
      std::thread([&]() { matches_[match_id]->StartGame(); });
}

void SimpleLobby::WaitForJoin(int player_id) {
  waiting_players_[player_id]->RegisterForMessage(std::bind(
        &SimpleLobby::JoinRequest, this, player_id, std::placeholders::_1));
}
