#ifndef SHAPES_H
#define SHAPES_H

#include <iostream>
#include <vector>
#include <list>
#include <array>

template<typename LengthType>
class Triangle
{
public:
    Triangle(LengthType len_a, LengthType len_b, LengthType len_c)
        : lengths_ { len_a, len_b, len_c }
    {
    }

    ~Triangle() { }
    

    LengthType perimeter() const
    {
        LengthType p = 0;
        for(LengthType elem : lengths_)
            p += elem;
        return p;
    }
    
    LengthType num_sides() const
    { return triangle_size; }        
    bool is_equilateral()
    {
        LengthType first_len = *lengths_.begin();
        for(int i = 1; i < triangle_size; ++i)
        {
            if(first_len != lengths_[i])
                return false;
        }
        return true;
    }
    
    
private:
    static constexpr int triangle_size = 3;
    std::array<LengthType, triangle_size> lengths_;    
};


/* class Rectangle : public Shape */
/* { */
/* public: */
/*     Rectangle(int len_vert, int len_horiz); */

/*     int perimeter() const override; */
/*     int num_sides() const override */
/*     { return rectangle_size; } */
    
/* private: */
/*     static constexpr int rectangle_size = 4; */
/*     int len_vert_, len_horiz_; */
/* }; */


#endif
