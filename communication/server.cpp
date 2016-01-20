#include "communication/server.h"

using boost::asio::ip::tcp;

Server::Server(boost::asio::io_service* io_service, int port, unique_ptr<Lobby> lobby)
  : io_service_(io_service),
    acceptor_(*io_service_, tcp::endpoint(tcp::v4(), port)),
    lobby_(std::move(lobby)) {
  StartAccept();
}

void Server::StartAccept() {
  // TODO(pzurkowski) Currently using raw pointer, as ASIO requires handlers
  // to be copyable, not just movable.
  tcp::socket* socket = new tcp::socket(*io_service_);
  acceptor_.async_accept(*socket,
      std::bind(&Server::HandleAccept, this, socket, std::placeholders::_1));
}

void Server::HandleAccept(tcp::socket* socket, const boost::system::error_code& error) {
  if (!error) {
    // TODO(pzurkowski) Use PlayerFactory here.
    unique_ptr<Connection> connection(new TcpConnection(std::move(*socket)));
    lobby_->AddPlayer(std::move(connection));
  } else {
    delete socket;
  }
  StartAccept();
}
