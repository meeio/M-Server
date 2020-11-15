#include <arpa/inet.h>
#include <iostream>
#include <spdlog/fmt/fmt.h>
#include <stdio.h>

typedef int port_t;

class B
{
public:
    B()
    {
        std::cout << "default" << std::endl;
    }

    B(B& other)
    {
        std::cout << "copy" << std::endl;
    }
};

class A
{
public:
    A() = delete;

    A(B& b)
    : b_(b)
    {

    }

    B b_;
};

int main()
{
    B bbb;
    A c(bbb);
}