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
  virtual ~RoundBasedPlayer();

  void Write(const Message& m) { connection_->Write(m); }

  // Returns a future referring to a message with given "round" set
  std::future<unique_ptr<Message>> GetRoundMessageAsync(int round);

 private:
  void ProcessMessage(unique_ptr<Message> message);
  void WaitForMessageAsync();

  std::mutex mtx_;
  std::map<int, std::promise<unique_ptr<Message>>> waiting_rounds_;

  vector<unique_ptr<Message>> waiting_messages_;

  unique_ptr<Connection> connection_;
};

#endif  // GAMES_ROUND_BASED_PLAYER_H_
