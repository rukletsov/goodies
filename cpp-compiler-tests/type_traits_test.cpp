
#include <iomanip>
#include <iostream>
#include <type_traits>


class A {
  A(const A&);
};
class B : public A {};

class C {};
class D : private C {};


int main() {
    std::cout << std::boolalpha;

    std::cout << "B* is convertible to A*: " << std::is_convertible<B*, A*>::value << std::endl;
    std::cout << "B is convertible to A: " << std::is_convertible<B, A>::value << std::endl;
    std::cout << "A is base of B: " << std::is_base_of<A, B>::value << std::endl;

    std::cout << "D* is convertible to C*: " << std::is_convertible<D*, C*>::value << std::endl;
    std::cout << "D is convertible to C: " << std::is_convertible<D, C>::value << std::endl;
    std::cout << "C is base of D: " << std::is_base_of<C, D>::value << std::endl;

    return 0;
}
