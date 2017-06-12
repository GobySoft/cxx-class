#include "shapes.h"

Triangle::Triangle(int len_a, int len_b, int len_c)
    : lengths_ { len_a, len_b, len_c }
{
    std::cout << "Constructing a triangle" << std::endl;
    
}

Triangle::~Triangle()
{
    std::cout << "Destructing a triangle" << std::endl;
}

int Triangle::perimeter() const
{
    int p = 0;
    for(int elem : lengths_)
        p += elem;
    return p;
}

bool Triangle::is_equilateral()
{
    int first_len = *lengths_.begin();
    for(int i = 1; i < triangle_size; ++i)
    {
        if(first_len != lengths_[i])
            return false;
    }
    return true;        
}

