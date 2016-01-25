#include "games/round_based/player.h"

RoundBasedPlayer::RoundBasedPlayer(unique_ptr<Connection> connection)
    : connection_(std::move(connection)) {
  WaitForMessageAsync();
}
RoundBasedPlayer::~RoundBasedPlayer() {
  // If this is called explicitly, the pointer is set to null, before the
  // destructor is called, therefore it wont be listened to again.
  connection_.reset();
}

std::future<unique_ptr<Message>> RoundBasedPlayer::GetRoundMessageAsync(int round) {
  std::lock_guard<std::mutex> guard(mtx_);
  waiting_rounds_[round] = std::promise<unique_ptr<Message>>();
  return waiting_rounds_[round].get_future();
}

void RoundBasedPlayer::ProcessMessage(unique_ptr<Message> message) {
  if (message->content().has_member("round")) {
    std::lock_guard<std::mutex> guard(mtx_);
    int round = message->content().get("round").as_int();
    if (waiting_rounds_.count(round)) {
      waiting_rounds_[round].set_value(std::move(message));
      waiting_rounds_.erase(round);
    } else {
      // TODO(pzk) Handle past messages
      waiting_messages_.push_back(std::move(message));
    }
  }
  WaitForMessageAsync();
}

void RoundBasedPlayer::WaitForMessageAsync() {
  if (connection_) {
    connection_->ReadMessageAsync(
        std::bind(&RoundBasedPlayer::ProcessMessage, this, std::placeholders::_1));
  }
}
