#ifndef GAMES_SNAKE_MATCH_H_
#define GAMES_SNAKE_MATCH_H_

#include "common/declarations.h"

#include "games/match.h"

class SnakeMatch : public Match {
 public:
  struct Options {
    int num_players;
  };

  explicit SnakeMatch(const Options& options);

  virtual void StartGame() override;
};

#endif  // GAMES_SNAKE_MATCH_H_
