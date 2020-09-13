
#include <atomic>
#include <gtest/gtest.h>
#include <iostream>

#include <threadpool.hpp>

using namespace std;

class ThreadPoolTestP: public testing::TestWithParam<tuple<size_t, size_t>>
{ };

TEST(ThreadPoolTest, SingleTask)
{
  ThreadPool tp(3);

  int var = 0;
  atomic<bool> ready = false;

  auto task = [&var, &ready]() {
    var = 42;
    ready.store(true, memory_order_release);
  };

  tp.assign(task);

  while (!ready.load(memory_order_acquire)) {
    this_thread::yield();
  }

  EXPECT_EQ(var, 42);
}

TEST_P(ThreadPoolTestP, MultipleTasks)
{
  const size_t pool_size = get<0>(GetParam());
  const size_t task_number = get<1>(GetParam());

  ThreadPool tp(pool_size);

  atomic<int> counter = 0;

  auto task = [&counter]() {
    counter.fetch_add(1, memory_order_release);
  };

  for (size_t i = 0; i < task_number; ++i) {
    tp.assign(task);
  }

  while(!tp.is_idle()) {
    this_thread::sleep_for(chrono::milliseconds(100));
  }

  EXPECT_EQ(counter.load(memory_order_acquire), task_number);
}

INSTANTIATE_TEST_SUITE_P(
  PoolSizeAndTaskNumber,
  ThreadPoolTestP,
  ::testing::Combine(
    ::testing::Values(1, 2, 5),
    ::testing::Values(5, 10, 50)));
