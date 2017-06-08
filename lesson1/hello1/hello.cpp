#include <iostream>

bool hello();

int main() 
{
    if(hello())
        return 0;
    else
        return 1;
}

bool hello()
{
    std::cout << "Hello world!" << std::endl;
    return true;
}

