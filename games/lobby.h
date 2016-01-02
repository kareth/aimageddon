#ifndef GAMES_LOBBY_H_
#define GAMES_LOBBY_H_

#include <thread>

#include "common/declarations.h"
#include "common/player.h"
#include "games/match.h"
#include "games/snake/match.h"

class Lobby {
 public:
  virtual ~Lobby() {}
  virtual void AddPlayer(unique_ptr<Player> player) = 0;
};

// Simple lobby, assigning players sequentialy to awaiting games.
// Whenever there is no matching game, new one is queued.
class SequentialLobby : public Lobby {
 public:
  explicit SequentialLobby(unique_ptr<MatchFactory> match_factory);

  // Joins with all threads.
  virtual ~SequentialLobby();

  // Adds a player and automatically starts a single player game.
  void AddPlayer(unique_ptr<Player> p);

 private:
  // Consider join request from a player.
  // Continues waiting if the message is not a join message.
  void JoinRequest(int player_id, unique_ptr<Message> message);

  // Observes a player for a join message.
  void WaitForJoin(int player_id);

  // Scans existing matches for a compatible candidate, and joins if possible.
  bool TryJoinExistingMatch(int player_id, const Json& match_options);

  // Starts a match in a separate thread.
  void StartMatch(int match_id);

  // Assigns player to a match, removing it from list of waiting players.
  void AssignPlayerToMatch(int player_id, int match_id);

  map<int, unique_ptr<Player>> waiting_players_;
  map<int, unique_ptr<Match>> matches_;
  map<int, std::thread> match_threads_;
  int player_id_counter_ = 0;
  int match_id_counter_ = 0;

  unique_ptr<MatchFactory> match_factory_;
};

#endif  // GAMES_LOBBY_H_
