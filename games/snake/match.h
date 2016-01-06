#ifndef GAMES_SNAKE_MATCH_H_
#define GAMES_SNAKE_MATCH_H_

#include <deque>
#include <set>

#include "common/declarations.h"
#include "common/grid.h"
#include "games/match.h"
#include "games/round_based/match.h"

class SnakeMatchFactory : public MatchFactory {
 public:
  SnakeMatchFactory();
  ~SnakeMatchFactory() {}
  virtual unique_ptr<Match> CreateMatch(const Json& match_params) override;
};

class SnakeMatch : public RoundBasedMatch {
 public:
  struct Options {
    // Returns 0 if the parse is correct, 1 if error.
    int ParseFromJson(const Json& json);

    // Returns true if options are equal.
    bool IsCompatible(const Options& rhs);

    int num_players;
    int x = 15;
    int y = 15;
    int starting_length = 4;
    int max_turns = 20;
    int turns_to_starve = 0;
    int action_time = 1000;
  };

  explicit SnakeMatch(const Options& options);
  virtual ~SnakeMatch() {}

  virtual bool CheckOptionsCompatibility(const Json& match_options) override;
  virtual void StartGame(std::function<void()> finish_callback) override;

 private:
  enum Action { kLeft, kRight, kForward };
  enum SpecialFields { kEmpty = -1, kApple = -2 };
  struct Snake {
    bool dead = false;
    std::deque<Point> points;
    Point direction;
  };

  // Generates initial snakes positions.
  // Each snake is generated as a line parallel to one edge of the map.
  // It starts in the corner, 1 tile off the edges, and spans across one direction.
  void SpawnSnakes();
  bool TestEndGame();

  Message MakeGameStart();
  Message MakeGameEnd();
  Message MakeGameStatus();
  vector<SnakeMatch::Action> FetchActions();
  void ProcessTurn(const vector<Action>& actions);
  Action ParseAction(const Message& message);

  // Temporary method to deal with recording games, before actual pubsub is setup
  Message Record(Message m) { game_log_.push_back(m); return m; }

  int current_turn_ = 1;
  Grid<int> board_;
  vector<Snake> snakes_;
  std::set<Point> apples_;
  vector<Json> events_;

  vector<Message> game_log_;
  Options options_;
};

#endif  // GAMES_SNAKE_MATCH_H_
