#include <arpa/inet.h>
#include <stdio.h>
#include <iostream>

class A
{
public:
    A() = default;

    A(const A& ref)
    {
        std::cout << "copy" << std::endl;
    }

    A get_ref()
    {
        return *this;
    }

};

int main()
{
    int a = 1;
    int* pa = &a;
    auto pb = reinterpret_cast<float*>(pa);
    a ++;
    std::cout << &(*pa) << std::endl;
    std::cout << &(*pb) << std::endl;
    std::cout << pa << std::endl;
    std::cout << pb << std::endl;
}