#include <iostream>
#include <reflect.h>
#include <traits/trait.h>


class Base
{
public:
    explicit Base(size_t n) : val(n) {}

    void con(std::string s) const {}

    const int val;
};

int add()
{
    return 0;
}

class A
{
public:
    const decltype(Base::con) con;
    const decltype(&add) add;

};



using namespace std;

//REGISTER_CLASS(Base, size_t);
//REGISTER_CLASS(Base, string);

int main()
{
    using l1 = Amazing::Reflection::type_list<int, double, char>;
    using l2 = Amazing::Reflection::type_list<int, std::string, float>;

    static constexpr auto i = Amazing::Reflection::is_convertible_v<l1, l2>;

    auto ii = std::is_constructible_v<Base, std::string>;

    auto a = &Base::val;

    Base base(1);

    auto nnn = &A::add;

    //Amazing::Reflection::variable_traits<decltype(Base::con)>;

    Amazing::Reflection::variable_traits<decltype(nnn)>;

    std::is_member_function_pointer_v<decltype(Base::con)>;

    auto b = base.val;


    //auto i = CREATE_CLASS(Base, "con");

    std::cout << "Hello, World!" << std::endl;
    return 0;
}