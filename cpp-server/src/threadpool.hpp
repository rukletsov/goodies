#ifndef CPP_SERVER_THREADPOOL_HPP
#define CPP_SERVER_THREADPOOL_HPP

#include <atomic>
#include <cstdint>
#include <deque>
#include <memory>
#include <mutex>
#include <set>

#include <worker.hpp>

class ThreadPool
{
public:
  using Task = std::function<void(void)>;

  ThreadPool(std::size_t size);
  virtual ~ThreadPool();

  void assign(const Task& task);
  bool is_idle();

private:
  void try_find_worker();

  void shutdown();

private:
  using Workers = std::set<std::shared_ptr<Worker>>;

  std::shared_ptr<std::mutex> m_;

  const size_t size_;
  Workers idle_;
  Workers busy_;

  std::deque<Task> tasks_;
};

#endif // CPP_SERVER_THREADPOOL_HPP
