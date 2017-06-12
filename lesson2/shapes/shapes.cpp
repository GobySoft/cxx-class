#include "shapes.h"

#include <algorithm>
#include <map>

void shape_info(const Shape& shape)
{
    std::cout << "my shape has " << shape.num_sides() << " sides" << std::endl;
    std::cout << "perimeter is : " << shape.perimeter() << std::endl;
}


int main(int argc, char* argv[])
{
    std::cout << "Starting up" << std::endl;

    Triangle tri_a(2, 2, 2);
    Triangle tri_b(3,4,5);
    Rectangle rect_c(3,5);
    std::cout << "perimeter of my tri_a is " << tri_a.perimeter() << std::endl;
    std::cout << "tri_a is equilateral? " << std::boolalpha << tri_a.is_equilateral() << std::endl;    
    std::cout << "tri_b is equilateral? " << std::boolalpha << tri_b.is_equilateral() << std::endl;
    std::cout << "perimeter of rect_c is " << rect_c.perimeter() << std::endl;
    
    std::vector<Shape*> shapes;
    shapes.push_back(&tri_a);
    shapes.push_back(&tri_b);
    shapes.push_back(&rect_c);

    for(Shape* shape_ptr : shapes)
    {
        shape_info(*shape_ptr);
    }

    std::map<char, Shape*> shape_map;
    shape_map.insert(std::make_pair('C', &rect_c));
    shape_map['A'] = &tri_a;
    shape_map['B'] = &tri_b;

    std::cout << "Shape A has perimeter of " << shape_map['A']->perimeter() << std::endl;
    
    std::cout << "Leaving main" << std::endl;
}

