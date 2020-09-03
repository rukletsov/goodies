
#include <gtest/gtest.h>
#include <iostream>
#include <thread>

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
  int var = 0;

  worker_.try_assign([&var](){
    var = 1;
  });

  wait();
  stop();

  EXPECT_EQ(var, 1);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
