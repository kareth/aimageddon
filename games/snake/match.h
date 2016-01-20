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

struct Snake {
  int id;
  bool dead = false;
  std::deque<Point> points;
  Point direction;

  void ShortenTail() {
    recently_removed_point_ = points.back();
    points.pop_back();
  }
  void RestoreTail() { points.push_back(recently_removed_point_); }

 private:
  Point recently_removed_point_;
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
    int max_turns = 40;
    int turns_to_starve = 0;
    int action_time = 1000;
  };

  enum Action { kLeft, kRight, kForward };

  explicit SnakeMatch(const Options& options);
  virtual ~SnakeMatch() {}

  virtual bool CheckOptionsCompatibility(const Json& match_options) override;
  virtual void StartGame(std::function<void()> finish_callback) override;

 private:
  enum SpecialFields { kEmpty = -1, kApple = -2 };

  // Generates initial snakes positions.
  // Each snake is generated as a line parallel to one edge of the map.
  // It starts in the corner, 1 tile off the edges, and spans across one direction.
  void SpawnSnakes();
  void ProcessTurn(const vector<Action>& actions);
  bool IsGameFinished();

  void KillSnake(int snake_id);
  void AppleEaten(int snake_id);

  Message MakeGameStart();
  Message MakeGameStatus();
  Message MakeGameEnd();

  int current_turn_ = 1;
  Grid<int> board_;
  vector<Snake> snakes_;
  std::set<Point> apples_;
  vector<Json> events_;

  vector<Message> game_log_;
  Options options_;
};

#endif  // GAMES_SNAKE_MATCH_H_
