#include "worker.hpp"

#include <iostream>

Worker::Worker(): thread_(&Worker::entrypoint_, this) { }

bool Worker::is_free()
{
  std::lock_guard guard(m_);
  return (!f_);
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
    } else {
      std::this_thread::yield();
    }
  }
}

void Worker::shutdown() {
  terminating_ = true;
}
