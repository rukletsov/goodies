#include "worker.hpp"

#include <iostream>

Worker::Worker(): state_(State::IDLE), thread_(&Worker::entrypoint_, this) { }

void Worker::entrypoint_()
{
  while(true) {
    State state;

    {
      std::lock_guard guard(m_);
      state = state_;
    }

    switch (state) {
      case State::IDLE:
        std::this_thread::yield();
        break;

      case State::BUSY:
        std::cout << " - " << std::this_thread::get_id() << " starting task" << std::endl;
        task_();
        std::cout << " - " << std::this_thread::get_id() << " finished task" << std::endl;
        transition_state_(State::IDLE);
        break;

      case State::STOPPED:
        std::cout << " - " << std::this_thread::get_id() << " stopping" << std::endl;
        return;
    }
  }
}

void Worker::transition_state_(State new_state)
{
  std::lock_guard guard(m_);

  if (state_ == State::STOPPED) return;

  state_ = new_state;
}
