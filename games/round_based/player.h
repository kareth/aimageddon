#ifndef GAMES_ROUND_BASED_PLAYER_H_
#define GAMES_ROUND_BASED_PLAYER_H_

#include <future>
#include <map>

#include "common/declarations.h"
#include "communication/connection.h"
#include "communication/message.h"

class RoundBasedPlayer {
 public:
  explicit RoundBasedPlayer(unique_ptr<Connection> connection);

  void Write(const Message& m) { connection_->Write(m); }

  std::future<unique_ptr<Message>> GetRoundMessageAsync(int round);

 private:
  void ProcessMessage(unique_ptr<Message> message);
  void WaitForMessageAsync();

  vector<unique_ptr<Message>> waiting_messages_;
  std::map<int, std::promise<unique_ptr<Message>>> waiting_rounds_;

  unique_ptr<Connection> connection_;
};

#endif  // GAMES_ROUND_BASED_PLAYER_H_
