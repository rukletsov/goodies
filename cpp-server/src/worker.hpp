#ifndef CPP_SERVER_WORKER_HPP
#define CPP_SERVER_WORKER_HPP

#include <functional>
#include <mutex>
#include <thread>

class Worker
{
public:
  template<typename T>
  using TaskP = std::function<T(void)>;
  using Task = std::function<void(void)>;

  template<typename T>
  using CallbackP = std::function<void(T)>;
  using Callback = std::function<void(void)>;

public:
  Worker();

  bool is_free();

  bool try_assign(const Task& task, const Callback& callback = Callback())
  {
    auto taskp = [task]() { task(); return Nothing(); };
    return callback
      ? try_assign<Nothing>(taskp, [callback](Nothing) { callback(); })
      : try_assign<Nothing>(taskp);
  }

  // Callback is called _after_ the worker is marked as ready to have assigned.
  // This is to allow assigning a new task in the callback.
  template<typename T>
  bool try_assign(const TaskP<T>& task, const CallbackP<T>& callback = CallbackP<T>())
  {
    std::lock_guard guard(m_);
    if (f_) {
      return false;
    } else {
      f_ = [task, callback, this](){
        T result = task();

        F empty;
        {
          std::lock_guard guard(m_);
          std::swap(f_, empty);
        }

        if (callback)
          callback(result);
      };

      return true;
    }
  }

  void wait() { thread_.join(); }
  void shutdown();

private:
  using F = std::function<void(void)>;
  class Nothing { };

  void entrypoint_();

private:
  F f_;

  std::atomic<bool> terminating_ = false;
  std::mutex m_;
  std::thread thread_;
};

#endif // CPP_SERVER_WORKER_HPP
