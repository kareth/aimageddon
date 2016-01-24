#ifndef COMMUNICATION_GAME_LOGGER_H_
#define COMMUNICATION_GAME_LOGGER_H_

#include "common/declarations.h"
#include "communication/message.h"
#include "communication/redis_client.h"
#include "cpp_redis/cpp_redis"

// Class used to log the progress of a game.
class GameLogger {
 public:
  virtual void Log(const Message& message) = 0;
};

class RedisGameLogger : public GameLogger {
 public:
  RedisGameLogger(int game_id, RedisClient* client);
  virtual void Log(const Message& message) override;

 private:
  string channel_;
  RedisClient* client_;
};

// Factories
class GameLoggerFactory {
 public:
  virtual unique_ptr<GameLogger> Create(int game_id) = 0;
};

class RedisGameLoggerFactory : public GameLoggerFactory {
 public:
  RedisGameLoggerFactory(RedisClient* client);
  virtual unique_ptr<GameLogger> Create(int game_id) override;

 private:
  RedisClient* client_;
};

#endif  // COMMUNICATION_GAME_LOGGER_H_
