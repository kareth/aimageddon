#ifndef COMMUNICATION_SERVER_H_
#define COMMUNICATION_SERVER_H_

#include <boost/asio.hpp>

#include "common/declarations.h"
#include "games/lobby.h"
#include "communication/tcp_player.h"

class Server {
 public:
  // Creates an acceptor on port @port and starts accepting connections.
  Server(boost::asio::io_service* io_service, int port, unique_ptr<Lobby> lobby);

 private:
  // Starts accepting connections.
  void StartAccept();

  // Handles a connection on a given socket.
  // Creates a player and adds it to a lobby.
  void HandleAccept(boost::asio::ip::tcp::socket* socket,
                    const boost::system::error_code& error);

  boost::asio::io_service* io_service_;
  boost::asio::ip::tcp::acceptor acceptor_;
  unique_ptr<Lobby> lobby_;
};

#endif  // COMMUNICATION_SERVER_H_
