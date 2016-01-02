#ifndef COMMUNICATION_TCP_PLAYER_H_
#define COMMUNICATION_TCP_PLAYER_H_

#include <queue>

#include "boost/asio.hpp"

#include "common/declarations.h"
#include "common/player.h"

// Asynchronous TCP player.
class TcpPlayer : public Player {
 public:
  using Callback = std::function<void(unique_ptr<Message>)>;

  // Expects a socket with accepted connection.
  explicit TcpPlayer(boost::asio::ip::tcp::socket socket);
  virtual ~TcpPlayer() {}

  // Writes a message to a player
  virtual void Write(const Message& message) override;

  // Reads a single message from a player.
  // If player haven't sent any message since last read, nullptr is returned.
  virtual unique_ptr<Message> Read() override;

  // Register observer for a message.
  // 1) If a message is already waiting, calls a callback with earlies message
  //    and consumes it. The call is executed from the thread calling this method.
  // 2) If no messages are waiting, queues the callback. Once the message is received,
  //    calls the callback with the thread that received it and consumes the message.
  virtual void RegisterForMessage(Callback callback) override;

  virtual bool active() override { return !disconnected; }

 private:
  // Starts an async read on the socket.
  void ReadMessage();

  // Triggered once read is complete.
  void HandleRead(const boost::system::error_code& error, size_t bytes);

  // Handles an incoming message by either triggering an observer or queueing it.
  void ProcessMessage(unique_ptr<Message> message);

  boost::asio::ip::tcp::socket socket_;
  boost::asio::streambuf buffer_;

  std::mutex mutex_;
  std::queue<Callback> observers_;
  std::queue<unique_ptr<Message>> messages_;

  bool disconnected = false;
};

#endif  // COMMUNICATION_TCP_PLAYER_H_
