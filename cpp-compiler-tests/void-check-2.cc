void F() {}

template <typename T>
class Option {
  public:

  Option(const T &) {}

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
