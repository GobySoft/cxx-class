#include "shapes.h"
#include <type_traits>

template<typename T>
T sum(T a, T b)
{
    static_assert(std::is_arithmetic<T>(), "must be arithmetic type to call sum()");
    return a+b;
}

int main()
{
    {
        Triangle<int> a(3,4,5);
        Triangle<double> b(1.555555555,2,2.5);
        Triangle<float> c(1.5, 2, 2.5);
        
        std::cout << a.perimeter() << std::endl;
        std::cout << b.perimeter() << std::endl;
        std::cout << c.perimeter() << std::endl;
    }
    
    {
        int a = 2;
        int b = 3;
        std::cout << sum(a, b) << std::endl;
        //std::cout << difference(a,b) << std::endl;
        
        std::string s = "abc";
        std::string t = "def";
        //        std::cout << sum(s, t) << std::endl;
    }
    
    
}

