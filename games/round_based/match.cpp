#include "games/round_based/match.h"

RoundBasedMatch::RoundBasedMatch(int num_players)
    : num_players_(num_players) {
}

void RoundBasedMatch::AddPlayer(unique_ptr<Connection> connection) {
  players_.emplace_back(new RoundBasedPlayer(std::move(connection)));
}

void RoundBasedMatch::Broadcast(const Message& message) {
  for (auto& player : players_)
    player->Write(message);
}

vector<unique_ptr<Message>> RoundBasedMatch::WaitForMessages(
    int round, std::chrono::milliseconds time_span) {
  std::chrono::steady_clock::time_point timeout =
    std::chrono::steady_clock::now() + time_span;

  vector<std::future<unique_ptr<Message>>> future_messages;
  for (auto& player : players_)
    future_messages.push_back(player->GetRoundMessageAsync(round));

  vector<unique_ptr<Message>> messages;
  for (auto& future_message : future_messages) {
    if (future_message.wait_until(timeout) == std::future_status::timeout) {
      printf("Missed move from player %d\n", int(messages.size()));
      messages.push_back(nullptr);
    } else {
      messages.push_back(future_message.get());
    }
  }
  return messages;
}
