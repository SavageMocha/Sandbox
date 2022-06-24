#include <iostream>
#include "include.h"
#include "lib.h"

// main entry point for the project
int main(void)
{
    std::cout << "hello world\n";
    include::print();
    lib::print();

    return 0;
}