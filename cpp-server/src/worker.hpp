#ifndef CPP_SERVER_WORKER_HPP
#define CPP_SERVER_WORKER_HPP

#include <functional>
#include <mutex>
#include <thread>

class Worker
{
public:
  using Task = std::function<void(void)>;

  Worker();

  bool try_assign(const Task& task);

  void detach() { thread_.detach(); }
  void wait() { thread_.join(); }
  void shutdown();

private:
  void entrypoint_();

private:
  std::mutex m_;
  std::thread thread_;
  Task task_;
  std::atomic<bool> terminating_;
};

#endif // CPP_SERVER_WORKER_HPP
