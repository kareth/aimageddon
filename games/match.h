#ifndef GAMES_MATCH_H_
#define GAMES_MATCH_H_

#include "common/declarations.h"
#include "communication/connection.h"
#include "communication/game_logger.h"
#include "communication/message.h"

class Match {
 public:
  Match();
  virtual ~Match() {}

  virtual bool CheckOptionsCompatibility(const Json& match_options) = 0;

  virtual void StartGame(std::function<void()> finish_callback) = 0;

  virtual void AddPlayer(unique_ptr<Connection> player) = 0;

  virtual bool is_full() = 0;

  void AddLogger(unique_ptr<GameLogger> logger) { logger_ = std::move(logger); }

 protected:
  void Log(const Message& message) { if (logger_) logger_->Log(message); }

 private:
  unique_ptr<GameLogger> logger_;
};

class MatchFactory {
 public:
  virtual ~MatchFactory() {}
  virtual unique_ptr<Match> CreateMatch(const Json& match_params) = 0;
};

#endif  // GAMES_MATCH_H_
