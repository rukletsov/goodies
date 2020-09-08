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

private:
  void try_find_worker();
  void process_notifications();

  void shutdown();

private:
  std::mutex m_;

  std::set<std::shared_ptr<Worker>> idle_;
  std::set<std::shared_ptr<Worker>> busy_;

  std::deque<Task> tasks_;

  // This is shared with workers
  std::shared_ptr<std::mutex> m_shared_;
  std::shared_ptr<std::deque<std::weak_ptr<Worker>>> notifications_;
};

#endif // CPP_SERVER_THREADPOOL_HPP
