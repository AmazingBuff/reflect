#include <iostream>
#include <reflect.h>
#include <traits/trait.h>
#include <traits/func.h>


class Base
{
public:
    explicit Base(size_t n) : val(n) {}

    explicit Base(const Base& b) : val(b.val) {}

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

void stt(std::string v)
{
    std::cout << v << std::endl;
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

void print(std::string f, double v)
{
    std::cout << f << ": " << v << std::endl;
}

template<typename T>
void printT(T&& t)
{
	std::cout << t << std::endl;
}


//REGISTER_CLASS(Base, size_t);
//REGISTER_CLASS(Base, string);

int main()
{
    using l1 = Amazing::Trait::type_list<int, double, char>;
    using l2 = Amazing::Trait::type_list<int, std::string, float>;
    //static constexpr auto i = Amazing::Trait::is_convertible_v<l1, l2>;

    std::tuple<int, char, double> t1(1, 'c', 1.0);
    std::tuple<int, char, double> t2(t1);

    std::tuple<std::tuple<int, char, double>, int, double> ttt(t1, 1, 2);

    auto ii = std::is_constructible_v<Base, std::string>;

    auto a = &Base::val;

    Base base(1);
    base.con("a");
    A ab;

    decltype(&Base::con) cc;

    Amazing::Trait::variable_traits<const void()>;

    Amazing::Trait::variable_traits<decltype(A::ad)>::parameter_type p;

    //bool y = std::is_same_v<decltype(p), decltype(Base::con)>;

    decltype(&A::accd) adc;
    decltype(A::accd) acdc;

    //decltype(&A::ad) dddd;

    decltype(&stt) dddd;

    auto iii = &Base::con;

    (base.*iii)("Hello, World!");

    Amazing::Trait::variable_traits<decltype(iii)>::type tttt = &stt;

    bool y = std::is_same_v<Amazing::Trait::variable_traits<decltype(&A::accd)>::type, decltype(&stt)>;

    Amazing::Trait::remove_qualifier_t<decltype(A::accd)> ll;

    //Amazing::Trait::is_function_pointer_v<decltype(A::ad)>;
    //Amazing::Trait::is_function_pointer_v<decltype(y)>;

    //bool t = std::is_same_v<Amazing::Trait::variable_traits<decltype(A::ad)>::parameter_type, Amazing::Trait::remove_qualifier_t<decltype(A::ad)>>;

    //std::is_member_function_pointer_v<decltype(Base::con)>;

    Amazing::Trait::type_list<int, char, double, std::string> list(1,1,1,"s");
    Amazing::Trait::type_list<int, char, double, std::string> list2(list);

    Amazing::Trait::type_list<Amazing::Trait::type_list<int, char, double, std::string>, int, double> list_l(list, 1, 2.0);

    //static_cast<Amazing::Trait::type_list<char, double, std::string>>(list);

    Amazing::Trait::remove_head_t<3, int, char, double, std::string> rrrr;

    Amazing::Trait::type_element<2, Amazing::Trait::type_list, int, char, double, std::string>::list rrr;

    //auto str = Amazing::Trait::get_value<3>(list);

    //auto v = Amazing::Reflection::get_value<3>("1", 1, 2, 5.f);

    Amazing::Trait::get_type_t<1, double, int>;

    std::is_invocable_v<decltype(print), std::string, double>;

    Amazing::Trait::reverse_type_t<3, int, char, double, std::string>;
    bool astf = Amazing::Trait::is_function_pointer<decltype(iii)>;


    //Amazing::Trait::apply(&::print, Amazing::Trait::type_list<std::string, double>("s", 10.0));

    Amazing::Trait::type_list<Base, int, int, std::string> tList(base, 1, 5, "Hello");

    Amazing::Trait::function_traits<decltype(iii)>::return_type;

    //std::invoke(iii, base, "Hello, World!");

    printT(iii);

    //Amazing::Reflection::invoke(iii, base, 'c');
    Amazing::Trait::invoke(&print, "Hello, World!", 2);

    //Amazing::Trait::apply(iii, base, Amazing::Trait::type_list<std::string>("ss"));

    Amazing::Trait::all_of(iii, base, "tu", Amazing::Trait::type_list<std::string>("ss"), "hhh");
    Amazing::Trait::all_of(&printT<std::string>, "tu", Amazing::Trait::type_list<std::string>("ss"), "hhh");
    Amazing::Trait::tail_type_t<int>;
    Amazing::Trait::overturn_types_t<int, char, int, double, float, std::string> ffff;

    std::tuple<std::string, double> tup("s", 1);
 

    Amazing::Trait::type_list<int, double> fDff(1, 2);


    Base f(base);

    auto b = base.val;

    ast("Hello, World!");

    std::invoke(iii, base, "Hello, World!");
    std::invoke(&stt, "Hello, World!");

    /*Amazing::Trait::invoke(&stt, "Hello, World!");
    Amazing::Trait::invoke(&Base::con, base, "Hello, Word!");*/

    //Amazing::Trait::function_traits<decltype(&Base::con)>::class_type bsb(2);

    std::is_member_function_pointer_v<decltype(iii)>;


    return 0;
}