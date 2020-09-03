#include "socket.hpp"

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>

const int QUEUE_LENGTH = 100;

std::optional<Socket*> Socket::open_socket(const std::uint16_t port)
{
  int s = socket(AF_INET, SOCK_STREAM, 0);
  if (s == -1) {
    std::cout << "Cannot create socket" << std::endl;
    return {};
  }

  sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  address.sin_addr.s_addr = INADDR_ANY;

  int bound = bind(s, (struct sockaddr*)&address, sizeof(address));
  if (bound == -1) {
    std::cout << "Cannot bind to port " << port << std::endl;
    return {};
  }

  int listened = listen(s, QUEUE_LENGTH);
  if (listened == -1) {
    std::cout << "Cannot listen for socket " << std::hex << s << std::endl;
    return {};
  }

  return new Socket(s);
}

Socket* Socket::accept()
{
  int s = ::accept(socket_, nullptr, nullptr);
  if (s == -1) {
    std::cout << "Accept on socket " << std::hex << " failed" << std::endl;
    return nullptr;
  }

  return new Socket(s);
}

Socket::Socket(int s): socket_(s)
{
  std::cout << "RAII wrapper for socket " << std::hex << s << " created" << std::endl;
}

Socket::~Socket() {
  std::cout << "Closing socket " << std::hex << socket_ << std::endl;
  close(socket_);
}
