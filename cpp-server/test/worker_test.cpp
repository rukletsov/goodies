
#include <atomic>
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
  std::atomic<bool> ready = false;

  cout << "Assign task" << endl;
  worker_.try_assign([&var](){
    var = 1;
  }, [&ready](){
    ready = true;
    cout << "Notified" << endl;
  });

  while (!ready) {
    cout << "Yielding" << endl;
    std::this_thread::yield();
  }

  cout << "Compare results" << endl;
  EXPECT_EQ(var, 1);
  cout << "Exit test" << endl;
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
