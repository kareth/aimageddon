#include "communication/tcp_player.h"

using boost::asio::ip::tcp;

TcpPlayer::TcpPlayer(tcp::socket socket)
    : socket_(std::move(socket)) {
  ReadMessage();
}

void TcpPlayer::Write(const Message& message) {
  string s = message.ToString();
  boost::asio::write(socket_, boost::asio::buffer(s.c_str(), s.size()));
}

unique_ptr<Message> TcpPlayer::Read() {
  std::lock_guard<std::mutex> guard(mutex_);
  if (messages_.size() == 0) return nullptr;

  auto msg = std::move(messages_.front());
  messages_.pop();
  return msg;
}

void TcpPlayer::RegisterForMessage(Callback callback) {
  unique_ptr<Message> message = nullptr;
  {
    std::lock_guard<std::mutex> guard(mutex_);
    if (messages_.size() > 0) {
      message = std::move(messages_.front());
      messages_.pop();
    } else {
      observers_.push(callback);
    }
  }
  if (message != nullptr) callback(std::move(message));
}

void TcpPlayer::ReadMessage() {
  boost::asio::async_read_until(socket_, buffer_, '\n',
    std::bind(&TcpPlayer::HandleRead, this, std::placeholders::_1, std::placeholders::_2));
}

void TcpPlayer::HandleRead(const boost::system::error_code& error, size_t bytes) {
  if (!error) {
    std::istream is(&buffer_);
    string message;
    std::getline(is, message);
    ProcessMessage(unique_ptr<Message>(new Message(message)));
    ReadMessage();
  } else {
    // TODO(pzurkowski) Handle errors.
    disconnected = true;
    socket_.close();
  }
}

void TcpPlayer::ProcessMessage(unique_ptr<Message> message) {
  /* Temporary */
  printf("MESSAGE: %s", message->ToString().c_str());
  string ping = "{\"type\":\"ping\"}";
  Write(Message(ping));
  /* Temporary */

  Callback callback;
  {
    std::lock_guard<std::mutex> guard(mutex_);
    if (observers_.size() > 0) {
      callback = observers_.front();
      observers_.pop();
    } else {
      messages_.push(std::move(message));
    }
  }
  if (callback != nullptr) callback(std::move(message));
}
