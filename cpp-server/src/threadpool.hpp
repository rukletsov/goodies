#ifndef CPP_SERVER_THREADPOOL_HPP
#define CPP_SERVER_THREADPOOL_HPP

#include <cstdint>
#include <deque>
#include <memory>
#include <mutex>
#include <set>

#include <worker.hpp>

class ThreadPool: std::enable_shared_from_this<ThreadPool>
{
public:
  using Task = std::function<void(void)>;

  ThreadPool(std::size_t size);
  virtual ~ThreadPool();

  void assign(const Task& task);

private:
  void try_find_worker();

private:
  std::mutex m_;

  std::set<std::shared_ptr<Worker>> idle_;
  std::set<std::shared_ptr<Worker>> busy_;

  std::deque<Task> tasks_;
};

#endif // CPP_SERVER_THREADPOOL_HPP
