#ifndef CPP_SERVER_THREADPOOL_HPP
#define CPP_SERVER_THREADPOOL_HPP

#include <cstdint>
#include <deque>
#include <memory>
#include <mutex>
#include <vector>

#include <worker.hpp>

class ThreadPool
{
public:
  using Task = std::function<void(void)>;

  explicit ThreadPool(std::size_t size);
  virtual ~ThreadPool();

  void assign(Task&& task);
  bool is_idle() const;

private:
  void try_find_worker();

  void shutdown();

private:
  using Workers = std::vector<std::shared_ptr<Worker>>;

  mutable std::mutex m_;

  Workers workers_;
  std::deque<Task> tasks_;
};

#endif // CPP_SERVER_THREADPOOL_HPP
