void F() {}

template <typename T>
class Option {
  public:

  Option(const T &) {}

  Option(T &&) {}
};

class Fork {
  public:

  Fork(const Option<void (*)(void)>&) {}

};


int main() {
  Fork fork(F);
}
