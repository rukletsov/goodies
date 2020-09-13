
#include <iostream>

size_t euler1()
{
  size_t sum = 0;
  for (size_t idx = 1; idx < 1000; ++idx) {
    if ((idx % 3 == 0) || (idx % 5 == 0)) {
      sum += idx;
    }
  }
  return sum;
}

size_t euler73()
{
  const size_t d = 12000;

  auto hcf = [](size_t high, size_t low) -> size_t {
    auto divident = high;
    auto divisor = low;
    decltype(divident) remainder;
    do {
      remainder = divident % divisor;
      divident = divisor;
      divisor = remainder;
    } while (remainder != 0);

    return divident;
  };

  size_t sum = 0;
  for (size_t idx = 5; idx <= d; ++idx) {
    auto x = idx / 3 + 1;
    auto y = idx / 2;

    for (size_t n = x; n <= y; ++n) {
      if (hcf(idx, n) == 1) { ++sum; }
    }
  }

  return sum;
}

int main()
{
  std::cout << "euler 1" << euler1() << std::endl;
  std::cout << "euler 73" << euler73() << std::endl;

  return 0;
}
