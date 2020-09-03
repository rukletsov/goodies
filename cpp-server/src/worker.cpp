#include "worker.hpp"

#include <iostream>

Worker::Worker(): terminating_(false), thread_(&Worker::entrypoint_, this) { }

bool Worker::try_assign(const Task& task, const Callback& callback)
{
  std::lock_guard guard(m_);
  if (task_) {
    return false;
  } else {
    task_ = task;
    callback_ = callback;
    return true;
  }
}

void Worker::entrypoint_()
{
  while(!terminating_) {
    std::cout << " > another futile round" << std::endl;
    Task task;
    {
      std::lock_guard guard(m_);
      task = task_;
    }

    if (task) {
      std::cout << " > Accepting task" << std::endl;
      task();
      std::cout << " > Finishing task" << std::endl;
      if (callback_)
        callback_();
      std::cout << " > Finishing notification" << std::endl;

      Task empty_task;
      Callback empty_callback;
      std::lock_guard guard(m_);
      std::swap(task_, empty_task);
      std::swap(callback_, empty_callback);
    } else {
      std::this_thread::yield();
    }
  }
}

void Worker::shutdown() {
  std::cout << " > Terminating" << std::endl;
  terminating_ = true;
}
