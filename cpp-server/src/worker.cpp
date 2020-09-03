#include "worker.hpp"

#include <iostream>

Worker::Worker(): terminating_(false), thread_(&Worker::entrypoint_, this) { }

bool Worker::try_assign(const Task& task)
{
  std::lock_guard guard(m_);
  if (task_) {
    return false;
  } else {
    task_ = task;
    return true;
  }
}

void Worker::entrypoint_()
{
  while(!terminating_) {
    Task task;
    {
      std::lock_guard guard(m_);
      task = task_;
    }

    if (task) {
      task();

      Task empty;
      std::lock_guard guard(m_);
      std::swap(task_, empty);
    } else {
      std::this_thread::yield();
    }
  }
}

void Worker::shutdown() {
  terminating_ = true;
}
