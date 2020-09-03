
#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include <optional>

#include <socket.hpp>
#include <worker.hpp>

int main()
{
//    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//    std::string hello("Hello world!");
//    std::cout << hello << std::endl;

//    auto c = 5;
//    std::cout << "auto int value: " << c << std::endl;

//    std::shared_ptr<int> d_ptr(new int(7));
//    std::cout << "shared_ptr value: " << *d_ptr << std::endl;

//    auto e = 1 << 2;
//    std::cout << e << std::endl;

//    std::pair p(0.5, true);
//    std::cout << p.first << std::boolalpha << " " << p.second << std::endl;

//    std::optional<int> ol(10);
//    ol.reset();
//    std::cout << bool(ol) << std::endl;

//  auto create = Socket::open_socket(8080);
//  std::shared_ptr<Socket> socket;
//  if (create) {
//    socket.reset(create.value());
//    auto s = socket->accept();
//  }

  Worker w;
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  w.try_assign([](){
    std::cout << " > Starting work" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    std::cout << " > Finishing work" << std::endl;
  });

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  std::cout << "Stopping" << std::endl;
  w.shutdown();
  w.wait();


  return 0;
}
