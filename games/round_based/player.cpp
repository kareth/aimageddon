#include "games/round_based/player.h"

RoundBasedPlayer::RoundBasedPlayer(unique_ptr<Connection> connection)
    : connection_(std::move(connection)) {
  WaitForMessageAsync();
}

std::future<unique_ptr<Message>> RoundBasedPlayer::GetRoundMessageAsync(int round) {
  waiting_rounds_[round] = std::promise<unique_ptr<Message>>();
  return waiting_rounds_[round].get_future();
}

void RoundBasedPlayer::ProcessMessage(unique_ptr<Message> message) {
  if (message->content().has_member("round")) {
    int round = message->content().get("round").as_int();
    if (waiting_rounds_.count(round)) {
      waiting_rounds_[round].set_value(std::move(message));
      waiting_rounds_.erase(round);
    } else {
      waiting_messages_.push_back(std::move(message));
    }
  }
  WaitForMessageAsync();
}

void RoundBasedPlayer::WaitForMessageAsync() {
  connection_->ReadMessageAsync(
      std::bind(&RoundBasedPlayer::ProcessMessage, this, std::placeholders::_1));
}
