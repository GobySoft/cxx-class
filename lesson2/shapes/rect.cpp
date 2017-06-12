#include "shapes.h"

Rectangle::Rectangle(int len_vert, int len_horiz)
    : len_vert_(len_vert),
      len_horiz_(len_horiz)
{
}

int Rectangle::perimeter() const
{
    return 2*len_vert_ + 2*len_horiz_;
}
