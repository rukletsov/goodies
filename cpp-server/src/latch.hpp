#ifndef CPP_SERVER_LATCH_HPP
#define CPP_SERVER_LATCH_HPP

#include <atomic>
#include <thread>

class Latch
{
public:
  Latch() { }

  void notify() { triggered_ = true; }
  void await() { while (!triggered_) { std::this_thread::yield(); } }

private:
  std::atomic<bool> triggered_ = false;
};

#endif // CPP_SERVER_LATCH_HPP
