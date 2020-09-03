#include "worker.hpp"

#include <iostream>

Worker::Worker(): terminating_(false), thread_(&Worker::entrypoint_, this) { }

bool Worker::try_assign(const Task& task, const Callback& callback)
{
  std::lock_guard guard(m_);
  if (f_) {
    return false;
  } else {
    f_ = [task, callback](){
      task();
      if (callback)
        callback();
    };

    return true;
  }
}

void Worker::entrypoint_()
{
  while(!terminating_) {
    F f;
    {
      std::lock_guard guard(m_);
      f = f_;
    }

    if (f) {
      f();

      F empty;
      std::lock_guard guard(m_);
      std::swap(f_, empty);
    } else {
      std::this_thread::yield();
    }
  }
}

void Worker::shutdown() {
  terminating_ = true;
}
