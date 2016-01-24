#include "games/lobby.h"

SequentialLobby::SequentialLobby(unique_ptr<MatchFactory> match_factory,
                                 unique_ptr<GameLoggerFactory> game_logger_factory)
    : match_factory_(std::move(match_factory)),
      game_logger_factory_(std::move(game_logger_factory)) {
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
  waiting_players_[player_id]->ReadMessageAsync(std::bind(
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
  printf("Erasing match %d\n", match_id);
  matches_.erase(match_id);
}

void SequentialLobby::JoinRequest(int player_id, unique_ptr<Message> message) {
  if (message->content().get("type") != "join_game") {
    WaitForJoin(player_id);
    return;
  }
  auto match_options = message->content().get("data", jsoncons::json(""));
  if (!TryJoinExistingMatch(player_id, match_options)) {
    // TODO(pzk) create logger here (with mutexed id)
    auto match = match_factory_->CreateMatch(match_options);
    if (match == nullptr) {
      WaitForJoin(player_id);
      // TODO(pzurkowski) what about error response?
    } else {
      std::lock_guard<std::mutex> guard(matches_mutex_);
      int match_id = ++match_id_counter_;

      // TODO(pzk) this is ugly
      match->AddLogger(game_logger_factory_->Create(match_id));

      matches_[match_id] = std::move(match);
      AssignPlayerToMatch(player_id, match_id);
      if (matches_[match_id]->is_full()) StartMatch(match_id);
    }
  }
}

bool SequentialLobby::TryJoinExistingMatch(int player_id, const Json& match_options) {
  std::lock_guard<std::mutex> guard(matches_mutex_);
  for (auto& p : matches_) {
    auto& match_id = p.first;
    auto& match = p.second;
    if (!match->is_full() && match->CheckOptionsCompatibility(match_options)) {
      AssignPlayerToMatch(player_id, match_id);
      if (match->is_full()) StartMatch(match_id);
      return true;
    }
  }
  return false;
}

void SequentialLobby::AssignPlayerToMatch(int player_id, int match_id) {
  matches_[match_id]->AddPlayer(std::move(waiting_players_[player_id]));
  waiting_players_.erase(player_id);
}
