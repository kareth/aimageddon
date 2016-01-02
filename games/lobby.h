#ifndef GAMES_LOBBY_H_
#define GAMES_LOBBY_H_

#include <thread>

#include "common/declarations.h"
#include "common/player.h"
#include "games/match.h"
#include "games/snake/match.h"

class Lobby {
 public:
  virtual void AddPlayer(unique_ptr<Player> player) = 0;
};

// Temporary implementation of example lobby.
class SimpleLobby : public Lobby {
 public:
  SimpleLobby();

  // Joins with all threads.
  ~SimpleLobby();

  // Adds a player and automatically starts a single player game.
  void AddPlayer(unique_ptr<Player> p);

 private:
  // Consider join request from a player.
  // Continues waiting if the message is not a join message.
  void JoinRequest(int player_id, unique_ptr<Message> message);

  // Observes a player for a join message.
  void WaitForJoin(int player_id);

  map<int, unique_ptr<Player>> waiting_players_;
  map<int, unique_ptr<Match>> matches_;
  map<int, std::thread> match_threads_;
  int player_id_counter_ = 0;
  int match_id_counter_ = 0;
};

#endif  // GAMES_LOBBY_H_
