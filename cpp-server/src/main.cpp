
#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include <optional>

#include <socket.hpp>
#include <worker.hpp>

int main()
{
  auto create = Socket::open_socket(8080);
  std::shared_ptr<Socket> socket;
  if (create) {
    socket.reset(create.value());
    auto s = socket->accept();
  }

  return 0;
}
