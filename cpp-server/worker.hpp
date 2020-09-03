#ifndef CPP_SERVER_WORKER_HPP
#define CPP_SERVER_WORKER_HPP

#include <functional>
#include <mutex>
#include <thread>

class Worker
{
public:
  Worker();

  template<typename T>
  bool try_assign(const T& functor)
  {
    std::lock_guard guard(m_);
    if (state_ != State::IDLE) {
      return false;
    }

    task_ = functor;
    state_ = State::BUSY;
    return true;
  }

  void wait() { thread_.join(); }
  void shutdown() { transition_state_(State::STOPPED); }

private:
  enum class State
  {
    IDLE,
    BUSY,
    STOPPED
  };

  void entrypoint_();
  void transition_state_(State new_state);

private:
  std::mutex m_;
  State state_;
  std::thread thread_;
  std::function<void(void)> task_;
};

#endif // CPP_SERVER_WORKER_HPP
