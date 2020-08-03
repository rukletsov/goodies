
#include <iostream>
#include <vector>


struct A {
  void touch() const { std::cout << "A" << std::endl; }
};

struct B {
  operator const std::vector<A>&() const { return v_; }
  void touch() const { std::cout << "B" << std::endl; }

  std::vector<A> v_ = {A(), A()};
};

template <typename T>
void process(const std::vector<T>& collection)
{
  for (const T& elem : collection)
  {
    elem.touch();
  }
}


int main() {
  B b;
  process(static_cast<std::vector<A>>(b));

  return 0;
}
