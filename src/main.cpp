#include <iostream>
#include <reflect.h>
#include <traits/trait.h>
#include <traits/func.h>


class Base
{
public:
    explicit Base(size_t n) : val(n) {}

    void con(std::string s) const 
    {
        std::cout << s << std::endl;
    }

    const int val;
};

int add()
{
    return 0;
}

struct A
{
public:
    A() : ad(add){}

    decltype(Base::con) accd;
    int f;

    int (* ad) ();
private:
};

void stt(std::string)
{
    std::cout << "Hello, World!" << std::endl;
}

void ast(std::string v)
{
    return stt(v);
}

int dda(std::string v)
{
    stt(v);
    return v.size();
}

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
    base.con("a");
    A ab;

    decltype(&Base::con) cc;

    Amazing::Reflection::variable_traits<const void()>;

    Amazing::Reflection::variable_traits<decltype(A::ad)>::parameter_type p;

    //bool y = std::is_same_v<decltype(p), decltype(Base::con)>;

    decltype(&A::accd) adc;
    decltype(A::accd) acdc;

    //decltype(&A::ad) dddd;

    decltype(&stt) dddd;

    auto iii = &Base::con;

    (base.*iii)("Hello, World!");

    Amazing::Reflection::variable_traits<decltype(iii)>::type tttt = &stt;

    bool y = std::is_same_v<Amazing::Reflection::variable_traits<decltype(&A::accd)>::type, decltype(&stt)>;

    Amazing::Reflection::remove_qualifier_t<decltype(A::accd)> ll;

    //Amazing::Reflection::is_function_pointer_v<decltype(A::ad)>;
    //Amazing::Reflection::is_function_pointer_v<decltype(y)>;

    //bool t = std::is_same_v<Amazing::Reflection::variable_traits<decltype(A::ad)>::parameter_type, Amazing::Reflection::remove_qualifier_t<decltype(A::ad)>>;

    //std::is_member_function_pointer_v<decltype(Base::con)>;

    Amazing::Reflection::type_list<int, char, double, std::string> list(1,1,1,"s");

    //static_cast<Amazing::Reflection::type_list<char, double, std::string>>(list);

    Amazing::Reflection::remove_head_t<3, int, char, double, std::string> rrrr;

    Amazing::Reflection::type_element<2, Amazing::Reflection::type_list, int, char, double, std::string>::list rrr;

    auto str = Amazing::Reflection::get_value<3>(list);


    Amazing::Reflection::reverse_type_t<3, int, char, double, std::string>;
    Amazing::Reflection::all_of(&dda, "ii", "dd", "ff");
    Amazing::Reflection::tail_type_t<int>;
    Amazing::Reflection::overturn_types_t<int, char, int, double, float, std::string> ffff;




    auto b = base.val;

    ast("Hello, World!");

    std::invoke(iii, base, "Hello, World!");
    std::invoke(&stt, "Hello, World!");

    Amazing::Reflection::invoke(&stt, "Hello, World!");
    Amazing::Reflection::invoke(&Base::con, base, "Hello, Word!");

    //Amazing::Reflection::function_traits<decltype(&Base::con)>::class_type bsb(2);

    std::is_member_function_pointer_v<decltype(iii)>;


    return 0;
}