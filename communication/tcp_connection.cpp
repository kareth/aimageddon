#include "communication/tcp_connection.h"

using boost::asio::ip::tcp;

TcpConnection::TcpConnection(tcp::socket socket)
    : socket_(std::move(socket)), should_continue_reading_(true) {
  ReadMessage();
}

TcpConnection::~TcpConnection() {
  std::unique_lock<std::mutex> guard(read_handle_mutex_);
  if (disconnected_ == false) {
    should_continue_reading_ = false;
    socket_.cancel();
    read_performed_.wait(guard);
  }
}

void TcpConnection::Write(const Message& message) {
  string s = message.ToString();
  printf("sending: %s", s.c_str());
  boost::asio::write(socket_, boost::asio::buffer(s.c_str(), s.size()));
}

unique_ptr<Message> TcpConnection::Read() {
  std::lock_guard<std::mutex> guard(queue_mutex_);
  if (messages_.size() == 0) return nullptr;

  auto msg = std::move(messages_.front());
  messages_.pop();
  return msg;
}

void TcpConnection::ReadMessageAsync(Callback callback) {
  unique_ptr<Message> message = nullptr;
  {
    std::lock_guard<std::mutex> guard(queue_mutex_);
    if (messages_.size() > 0) {
      message = std::move(messages_.front());
      messages_.pop();
    } else {
      observers_.push(callback);
    }
  }
  if (message != nullptr) callback(std::move(message));
}

void TcpConnection::ReadMessage() {
  boost::asio::async_read_until(socket_, buffer_, '\n',
    std::bind(&TcpConnection::HandleRead, this, std::placeholders::_1, std::placeholders::_2));
}

void TcpConnection::HandleRead(const boost::system::error_code& error, size_t bytes) {
  std::unique_lock<std::mutex> guard(read_handle_mutex_);
  if (!error) {
    std::istream is(&buffer_);
    string message;
    std::getline(is, message);
    ProcessMessage(unique_ptr<Message>(new Message(message)));
    if (should_continue_reading_)
      ReadMessage();
  } else {
    std::cout << "Reading error: " << error << std::endl;
    disconnected_ = true;
  }
  read_performed_.notify_one();
}

void TcpConnection::ProcessMessage(unique_ptr<Message> message) {
  printf("received: %s", message->ToString().c_str());

  Callback callback;
  {
    std::lock_guard<std::mutex> guard(queue_mutex_);
    if (observers_.size() > 0) {
      callback = observers_.front();
      observers_.pop();
    } else {
      messages_.push(std::move(message));
    }
  }
  if (callback != nullptr) callback(std::move(message));
}
