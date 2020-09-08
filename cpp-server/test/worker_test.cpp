
#include <gtest/gtest.h>
#include <iostream>
#include <thread>

#include <latch.hpp>
#include <worker.hpp>

using namespace std;


class WorkerTest: public ::testing::Test
{
protected:
  void TearDown() override
  {
    stop();
  }

  void stop()
  {
    if (!stopped_)
    {
      stopped_ = true;
      worker_.shutdown();
      worker_.wait();
    }
  }

  static void wait()
  {
    this_thread::sleep_for(chrono::milliseconds(5));
  }

  static void yield()
  {
    this_thread::yield();
  }

protected:
  Worker worker_;
  bool stopped_ = false;
};


TEST_F(WorkerTest, WorkerRunsTask)
{
  int var = 42;

  worker_.try_assign([&var]() {
    var += 1;
  });

  wait();

  EXPECT_EQ(var, 43);
}


TEST_F(WorkerTest, WorkerRunsTaskWithCallback)
{
  int var = 42;
  Latch n;

  worker_.try_assign([&var]() {
    var += 1;
  }, [&n]() {
    n.notify();
  });

  n.await();

  EXPECT_EQ(var, 43);
}


TEST_F(WorkerTest, WorkerRunsTaskParameterized)
{
  int var = 42;

  worker_.try_assign<int>([var]() -> int {
    return var + 1;
  });

  wait();

  EXPECT_EQ(var, 42);
}


TEST_F(WorkerTest, WorkerRunsTaskParameterizedWithCallback)
{
  int var;
  Latch n;

  worker_.try_assign<int>([]() -> int {
    return 42;
  }, [&n, &var](int value) {
    n.notify();
    var = value;
  });

  n.await();

  EXPECT_EQ(var, 42);
}

TEST_F(WorkerTest, WorkerRunsMultipleTasks)
{
  int var = 0;

  auto var_inc = [&var]() {
    yield();
    ++var;
    yield();
  };

  auto var_inc_ret = [](int* value) -> int {
    return *value + 1;
  };

  // 1. Assign
  bool assign1 = worker_.try_assign(var_inc);
  EXPECT_EQ(assign1, true);

  // 2. Wait in a spinlock on `is_free()` and then assign again.
  while (!worker_.is_free()) {
    yield();
  }
  EXPECT_EQ(var, 1);

  bool assign2 = worker_.try_assign(var_inc);
  EXPECT_EQ(assign2, true);

  // 3. Repeatedly try assign until success.
  Latch n;
  int res;
  while (!worker_.try_assign<int>(
           std::bind(var_inc_ret, &var),
           [&n, &res](int result) {
             res = result;
             n.notify();
           })) {
    yield();
  }

  n.await();

  EXPECT_EQ(var, 2);
  EXPECT_EQ(res, 3);
}
