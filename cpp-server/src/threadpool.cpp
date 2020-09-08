#include "threadpool.hpp"

#include <cassert>
#include <iostream>

using namespace std;

ThreadPool::ThreadPool(size_t size):
  m_shared_(make_shared<mutex>()),
  notifications_(make_shared<deque<weak_ptr<Worker>>>())
{
  for (size_t idx = 0; idx < size; ++idx) {
    idle_.emplace(make_shared<Worker>());
  }
}

ThreadPool::~ThreadPool()
{
  shutdown();
}

void ThreadPool::shutdown()
{
  lock_guard guard(m_);

  for (auto w: idle_) {
    w->shutdown();
    w->detach();
  }

  for (auto w: busy_) {
    w->shutdown();
    w->wait();
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

void ThreadPool::process_notifications()
{
  deque<weak_ptr<Worker>> notifications;

  {
    lock_guard nguard(*m_shared_);
    if (notifications_->empty()) return;
    notifications = *notifications_;
    notifications_->clear();
  }

  for (auto worker_weak : notifications) {
    if (!worker_weak.expired()) {
      auto worker = worker_weak.lock();

      lock_guard guard(m_);
      assert(busy_.count(worker) && "Finished worker was not busy");
      busy_.erase(worker);
      idle_.insert(worker);
    }
  }
}

void ThreadPool::try_find_worker()
{
  process_notifications();

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

  weak_ptr<Worker> worker_weak = worker;
  auto notifier = [m = m_shared_, notifier = notifications_, worker_weak]() {
    lock_guard nguard(*m);
    notifier->push_back(worker_weak);
  };

  bool assigned = worker->try_assign(task, notifier);
  assert(assigned && "Idle worker appears busy");

  {
    lock_guard guard(m_);
    busy_.insert(worker);
  }
}
