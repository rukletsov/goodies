#include "threadpool.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>

using namespace std;

ThreadPool::ThreadPool(size_t size)
{
  for (size_t idx = 0; idx < size; ++idx) {
    workers_.emplace_back(make_shared<Worker>());
  }
}

ThreadPool::~ThreadPool()
{
  shutdown();
}

bool ThreadPool::is_idle()
{
  lock_guard guard(m_);

  return (
    tasks_.empty() &&
    all_of(workers_.cbegin(), workers_.cend(), [](const auto& w) {
      return w->is_free();
    }));
}

void ThreadPool::shutdown()
{
  Workers workers;
  {
    lock_guard guard(m_);
    for (const auto& w: workers_) { w->shutdown(); }

    workers = workers_;
  }

  for (const auto& w: workers) { w->wait(); }
}

void ThreadPool::assign(const Task &task)
{
  {
    lock_guard guard(m_);
    tasks_.push_back(task);
  }
  try_find_worker();
}

void ThreadPool::try_find_worker()
{
  Task task;
  shared_ptr<Worker> worker;
  {
    lock_guard guard(m_);

    for (const auto& w : workers_) {
      if (w->is_free()) {
        worker = w;
        break;
      }
    }

    if (tasks_.empty() || !worker) {
      return;
    }

    // Now there are both a task and a free worker.

    task = tasks_.front();
    tasks_.pop_front();

    auto notifier = [this]() { try_find_worker(); };

    bool assigned = worker->try_assign(task, notifier);
    assert(assigned && "Idle worker appears busy");
  }
}
