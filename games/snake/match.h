#ifndef GAMES_SNAKE_MATCH_H_
#define GAMES_SNAKE_MATCH_H_

#include "common/declarations.h"

#include "games/match.h"

class SnakeMatchFactory : public MatchFactory {
 public:
  SnakeMatchFactory();
  ~SnakeMatchFactory() {}
  virtual unique_ptr<Match> CreateMatch(const Json& match_params) override;
};

class SnakeMatch : public Match {
 public:
  struct Options {
    // Returns 0 if the parse is correct, 1 if error.
    int ParseFromJson(const Json& json);

    // Returns true if options are equal.
    bool IsCompatible(const Options& rhs);

    int num_players;
  };

  explicit SnakeMatch(const Options& options);
  virtual ~SnakeMatch() {}

  virtual bool CheckOptionsCompatibility(const Json& match_options) override;
  virtual void StartGame() override;

 private:
  Options options_;
};

#endif  // GAMES_SNAKE_MATCH_H_
