#ifndef GAMES_ROUND_BASED_MATCH_H_
#define GAMES_ROUND_BASED_MATCH_H_

#include <chrono>

#include "common/declarations.h"
#include "communication/connection.h"
#include "communication/message.h"
#include "games/match.h"
#include "games/round_based/player.h"

class RoundBasedMatch : public Match {
 public:
  explicit RoundBasedMatch(int num_players);
  virtual ~RoundBasedMatch() {}

  virtual bool CheckOptionsCompatibility(const Json& match_options) = 0;

  virtual void StartGame(std::function<void()> finish_callback) = 0;

  virtual void AddPlayer(unique_ptr<Connection> connection) override;

  virtual bool is_full() override { return players_.size() == num_players_; }

 protected:
  void Broadcast(const Message& message);

  vector<unique_ptr<Message>> WaitForMessages(
      int round, std::chrono::milliseconds time_span);

  int num_players_;
  vector<unique_ptr<RoundBasedPlayer>> players_;
};

#endif  // GAMES_ROUND_BASED_MATCH_H_
