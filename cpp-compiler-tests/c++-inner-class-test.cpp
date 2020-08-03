
class A
{
public:
    class B
    {
    public:
       void test() {++number;}
    } b;

  void test() {b.test();}

  int number = 0;
};

int main()
{
    A a;

    return 0;
}
