
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

  void wait()
  {
    this_thread::sleep_for(chrono::milliseconds(5));
  }

protected:
  Worker worker_;
  bool stopped_ = false;
};


TEST_F(WorkerTest, WorkerCanRunTask)
{
  int var = 42;

  worker_.try_assign([&var]() {
    var += 1;
  });

  wait();

  EXPECT_EQ(var, 43);
}


TEST_F(WorkerTest, WorkerCanRunTaskWithCallback)
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


TEST_F(WorkerTest, WorkerCanRunTaskParameterized)
{
  int var = 42;

  worker_.try_assign<int>([var]() -> int {
    return var + 1;
  });

  wait();

  EXPECT_EQ(var, 42);
}


TEST_F(WorkerTest, WorkerCanRunTaskParameterizedWithCallback)
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


int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
