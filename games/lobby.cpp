#include "games/lobby.h"

SequentialLobby::SequentialLobby(unique_ptr<MatchFactory> match_factory)
    : match_factory_(std::move(match_factory)) {
}

SequentialLobby::~SequentialLobby() {
  for (auto& t : match_threads_) {
    t.second.join();
  }
}

void SequentialLobby::AddPlayer(unique_ptr<Connection> p) {
  int id = ++player_id_counter_;
  waiting_players_[id] = std::move(p);
  WaitForJoin(id);
}

void SequentialLobby::WaitForJoin(int player_id) {
  waiting_players_[player_id]->RegisterForMessage(std::bind(
        &SequentialLobby::JoinRequest, this, player_id, std::placeholders::_1));
}

void SequentialLobby::StartMatch(int match_id) {
  auto finish_callback = [this, match_id]() { MatchFinished(match_id); };
  match_threads_[match_id] =
      std::thread([this, match_id, finish_callback]() {
          matches_[match_id]->StartGame(finish_callback);
      });
}

void SequentialLobby::MatchFinished(int match_id) {
  std::lock_guard<std::mutex> guard(matches_mutex_);
  matches_.erase(match_id);
}

void SequentialLobby::JoinRequest(int player_id, unique_ptr<Message> message) {
  if (message->content().get("type") != "join_game") {
    WaitForJoin(player_id);
  }
  auto match_options = message->content().get("data", jsoncons::json(""));
  if (!TryJoinExistingMatch(player_id, match_options)) {
    auto match = match_factory_->CreateMatch(match_options);
    if (match == nullptr) {
      WaitForJoin(player_id);
      // TODO(pzurkowski) what about error response?
    } else {
      std::lock_guard<std::mutex> guard(matches_mutex_);
      int match_id = ++match_id_counter_;
      matches_[match_id] = std::move(match);
      AssignPlayerToMatch(player_id, match_id);
      if (matches_[match_id]->IsFull()) StartMatch(match_id);
    }
  }
}

bool SequentialLobby::TryJoinExistingMatch(int player_id, const Json& match_options) {
  std::lock_guard<std::mutex> guard(matches_mutex_);
  for (auto& p : matches_) {
    auto& match_id = p.first;
    auto& match = p.second;
    if (!match->IsFull() && match->CheckOptionsCompatibility(match_options)) {
      AssignPlayerToMatch(player_id, match_id);
      if (match->IsFull()) StartMatch(match_id);
      return true;
    }
  }
  return false;
}

void SequentialLobby::AssignPlayerToMatch(int player_id, int match_id) {
  matches_[match_id]->AddPlayer(std::move(waiting_players_[player_id]));
  waiting_players_.erase(player_id);
}
