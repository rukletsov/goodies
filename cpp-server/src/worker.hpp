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

  bool try_assign(const Task& task, const Callback& callback = Callback());

  template<typename T>
  bool try_assign(const TaskP<T>& task, const CallbackP<T>& callback = CallbackP<T>())
  {
    std::lock_guard guard(m_);
    if (f_) {
      return false;
    } else {
      f_ = [task, callback](){
        T result = task();
        if (callback)
          callback(result);
      };

      return true;
    }
  }

  void detach() { thread_.detach(); }
  void wait() { thread_.join(); }
  void shutdown();

private:
  using F = std::function<void(void)>;

  void entrypoint_();

private:
  std::mutex m_;
  std::thread thread_;
  F f_;
  std::atomic<bool> terminating_;
};

#endif // CPP_SERVER_WORKER_HPP
