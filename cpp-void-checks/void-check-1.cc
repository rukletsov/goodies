void F() {}

template <typename T>
class Option {
  public:
  Option(T &&) {}

  template <typename U>
  Option(const U &) {}

};

class Fork {
  public:

  Fork(const Option<void (*)(void)>&) {}

};


int main() {
  Fork fork(F);
}
