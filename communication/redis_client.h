#ifndef COMMUNICATION_REDIS_CLIENT_H_
#define COMMUNICATION_REDIS_CLIENT_H_

#include "common/declarations.h"
#include "cpp_redis/cpp_redis"

class RedisClient {
 public:
  RedisClient() {
     client_.connect();
     client_.set_disconnection_handler([] (cpp_redis::redis_client&) {
         // TODO(pzk) Add retry logic;
         printf("ERROR: Redis disconnected\n");
     });
  }

  void Send(const vector<string>& redis_cmd) { client_.send(redis_cmd); }

 private:
  cpp_redis::redis_client client_;
};

#endif  // COMMUNICATION_REDIS_CLIENT_H_
