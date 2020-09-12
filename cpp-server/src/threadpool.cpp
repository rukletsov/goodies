#include "threadpool.hpp"

#include <cassert>
#include <iostream>

using namespace std;

ThreadPool::ThreadPool(size_t size):
  size_(size),
  m_(make_shared<mutex>())
{
  for (size_t idx = 0; idx < size; ++idx) {
    idle_.emplace(make_shared<Worker>());
  }
}

ThreadPool::~ThreadPool()
{
  shutdown();
}

bool ThreadPool::is_idle()
{
  lock_guard guard(*m_);
  return (tasks_.empty() && busy_.empty() && (idle_.size() == size_));
}

void ThreadPool::shutdown()
{
  Workers workers;
  {
    lock_guard guard(*m_);

    for (auto w: idle_) { w->shutdown(); }
    for (auto w: busy_) { w->shutdown(); }

    workers.insert(idle_.cbegin(), idle_.cend());
    workers.insert(busy_.cbegin(), busy_.cend());
  }

  for (auto w: workers) { w->wait(); }
}

void ThreadPool::assign(const Task &task)
{
  {
    lock_guard guard(*m_);
    tasks_.push_back(task);
  }
  try_find_worker();
}

void ThreadPool::try_find_worker()
{
  Task task;
  shared_ptr<Worker> worker;
  {
    lock_guard guard(*m_);

    if (tasks_.empty() || idle_.empty()) {
      return;
    }

    task = tasks_.front();
    tasks_.pop_front();

    worker = *idle_.begin();
    idle_.erase(idle_.begin());
  }

  // Now there are both a task and a free worker.

  auto notifier = [worker, this]() {
    {
      lock_guard guard(*m_);
      assert(busy_.count(worker) && "Finished worker was not busy");
      busy_.erase(worker);
      idle_.insert(worker);
    }
    try_find_worker();
  };

  {
    lock_guard guard(*m_);
    busy_.insert(worker);
  }

  bool assigned = worker->try_assign(task, notifier);
  assert(assigned && "Idle worker appears busy");
}
