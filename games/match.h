#ifndef GAMES_MATCH_H_
#define GAMES_MATCH_H_

#include "common/declarations.h"
#include "common/player.h"

#include "communication/message.h"

class Match {
 public:
  explicit Match(int expected_players);

  virtual void StartGame() = 0;

  // Adds a player to a match.
  // If this is the last expected player, the game is automatically started
  void AddPlayer(unique_ptr<Player> player);

  // Returns true if the game is finished
  bool finished() { return finished_; }

  // Returns true if the game has enough connected players
  bool full() { return num_players_ = players_.size(); }

  bool players() { return players_.size(); }

 protected:
  // Sends a message to all players in a game
  void Broadcast(const Message& message);

  // Publishes data to an external pubsub queue.
  void Publish(const Message& message);

  int num_players_;
  vector<unique_ptr<Player>> players_;
  bool finished_ = false;
};

#endif  // GAMES_MATCH_H_
