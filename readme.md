# C++ Reflect Library
A c++ reflect library which use clang tooling to generate reflection information.
Every time you add an attribute `[[refl]]` to struct, class or enum, you can get the reflection information by adding function `EXECUTE_REFLECT` to the bottom of your cmake file.