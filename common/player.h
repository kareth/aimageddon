#ifndef COMMON_PLAYER_H_
#define COMMON_PLAYER_H_

#include "common/declarations.h"
#include "communication/message.h"

class Player {
 public:
  using Callback = std::function<void(unique_ptr<Message>)>;

  // Writes a message to a player.
  virtual void Write(const Message& message) = 0;

  // Reads a single message from a player.
  // If player haven't sent any message since last read, nullptr is returned.
  virtual unique_ptr<Message> Read() = 0;

  // Registers an observer for a message. Calls callback once message appears.
  virtual void RegisterForMessage(Callback callback) = 0;

  virtual bool active() = 0;
};

#endif  // COMMON_PLAYER_H_
