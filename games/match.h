#ifndef GAMES_MATCH_H_
#define GAMES_MATCH_H_

#include "common/declarations.h"
#include "communication/connection.h"
#include "communication/message.h"

// TODO(pzk) add unique id
class Match {
 public:
  Match();
  virtual ~Match();

  virtual bool CheckOptionsCompatibility(const Json& match_options) = 0;

  virtual void StartGame(std::function<void()> finish_callback) = 0;

  virtual void AddPlayer(unique_ptr<Connection> player) = 0;

  virtual bool is_full() = 0;

  virtual void Publish(const Message& message);

 private:
  // TODO(pzk) replace with redis channel
  FILE* log_file_ = nullptr;
  int written_ = 0;  // Temporary
};

class MatchFactory {
 public:
  virtual ~MatchFactory() {}
  virtual unique_ptr<Match> CreateMatch(const Json& match_params) = 0;
};

#endif  // GAMES_MATCH_H_
