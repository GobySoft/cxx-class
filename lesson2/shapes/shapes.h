#ifndef SHAPES_H
#define SHAPES_H

#include <iostream>
#include <vector>
#include <list>
#include <array>

class Shape
{
public:
    virtual int perimeter() const = 0;
    virtual int num_sides() const = 0;
};


class Triangle : public Shape
{
public:
    Triangle(int len_a, int len_b, int len_c);
    ~Triangle();

    int perimeter() const override;
    int num_sides() const override
    { return triangle_size; }        
    bool is_equilateral();
    
private:
    static constexpr int triangle_size = 3;
    std::array<int, triangle_size> lengths_;
    
};

class Rectangle : public Shape
{
public:
    Rectangle(int len_vert, int len_horiz);

    int perimeter() const override;
    int num_sides() const override
    { return rectangle_size; }
    
private:
    static constexpr int rectangle_size = 4;
    int len_vert_, len_horiz_;
};


class Square : public Rectangle
{
Square(int len) : Rectangle(len, len)
    { }
    
};



#endif
