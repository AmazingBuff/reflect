#include <iostream>
#include <reflect.h>
#include <traits/trait.h>
#include <traits/func.h>

#include <clang_plugin.h>
#include <clang/Tooling/Tooling.h>

const char* test = R"(namespace n { namespace m { class C {}; } })";

int main()
{
    clang::tooling::runToolOnCode(std::make_unique<Amazing::FindNamedClassAction>(), test);
}