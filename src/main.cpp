#include <iostream>
#include <reflect.h>


class Base
{
public:
    explicit Base(size_t n) {}
};

REGISTER_CLASS(Base, size_t);


int main()
{
    auto i = CREATE_CLASS(Base, 10);

    std::cout << "Hello, World!" << std::endl;
    return 0;
}