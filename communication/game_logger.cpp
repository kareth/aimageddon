#include "communication/game_logger.h"

RedisGameLogger::RedisGameLogger(int game_id, RedisClient* client)
    : client_(client) {
  channel_ = "match:" + std::to_string(game_id);
}

void RedisGameLogger::Log(const Message& message) {
  client_->Send({"PUBLISH", channel_, message.ToString()});
}

RedisGameLoggerFactory::RedisGameLoggerFactory(RedisClient* client)
    : client_(client) { }

unique_ptr<GameLogger> RedisGameLoggerFactory::Create(int game_id) {
  return unique_ptr<GameLogger>(new RedisGameLogger(game_id, client_));
}
