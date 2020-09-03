#ifndef CPP_SERVER_SOCKET_HPP
#define CPP_SERVER_SOCKET_HPP

#include <cstdint>
#include <optional>

class Socket
{
public:
  ~Socket();

  static std::optional<Socket*> open_socket(const std::uint16_t port);

  Socket* accept();

private:
  Socket(int s);
  Socket(const Socket&) = delete;
  Socket& operator=(const Socket&) = delete;

  int socket_;
};

#endif // CPP_SERVER_SOCKET_HPP
