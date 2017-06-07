#include <iostream>
#include "split.h"


// compile with
// g++ split-usage.cpp --std=c++11 -o split-usage

// run with
// ./split-usage
    
// output should be 
//   Talker: $TALKR
//   int in position 1: 4
//   double in position 1: 3.2

int main()
{
    std::string example = "$TALKR,4,3.2,45*14";

    NMEASentence nmea(example);

    enum { TALKER = 0, SOMEINT = 1, SOMEFLOAT = 2, UNUSED = 3 };
    
    std::cout << "Talker: " <<  nmea.as<std::string>(TALKER) << std::endl;

    int someint = nmea.as<int>(SOMEINT);
    std::cout << "int in position 1: " << someint << std::endl;

    double somefloat = nmea.as<double>(SOMEFLOAT);
    std::cout << "double in position 1: " << somefloat << std::endl;
}
