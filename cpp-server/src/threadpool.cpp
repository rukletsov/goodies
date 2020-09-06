#include "threadpool.hpp"

#include <cassert>

using namespace std;

ThreadPool::ThreadPool(size_t size)
{
  for (size_t idx = 0; idx < size; ++idx) {
    idle_.emplace(make_shared<Worker>());
  }
}

ThreadPool::~ThreadPool()
{
  for (auto w: idle_) {
    w->shutdown();
    w->detach();
  }

  for (auto w: busy_) {
    w->shutdown();
    w->detach();
  }
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
  {
    lock_guard guard(m_);
    if (tasks_.empty()) {
      return;
    } else {
      task = tasks_.front();
      tasks_.pop_front();
    }
  }

  shared_ptr<Worker> worker;

  {
    lock_guard guard(m_);
    for (auto wp = idle_.begin(); wp != idle_.end(); ++wp) {
      if ((*wp)->is_free()) {
        worker = *wp;
        idle_.erase(wp);
        break;
      }
    }
  }

  if (!worker) {
    lock_guard guard(m_);
    tasks_.push_front(task);
    return;
  }

  // Now there are both a task and a free worker.

  auto self_weak = weak_from_this();
  weak_ptr<Worker> worker_weak = worker;
  auto notifier = [self_weak, worker_weak]() {
    if (!self_weak.expired() && !worker_weak.expired()) {
      auto self = self_weak.lock();
      auto worker = worker_weak.lock();

      lock_guard guard(self->m_);
      assert(self->busy_.count(worker) && "Finished worker was not busy");
      self->busy_.erase(worker);
      self->idle_.insert(worker);
    }
  };

  bool assigned = worker->try_assign(task, notifier);
  assert(assigned && "Idle worker appears busy");

  {
    lock_guard guard(m_);
    busy_.insert(worker);
  }
}
