#ifndef POSITION_H_INCLUDED 
#define POSITION_H_INCLUDED

#include <vector>
#include "vertex.hpp"
#include <cstdlib>

using namespace std;

template<typename T>
class Position {
    private:
    T _x;
    T _y;

    public:
    Position(T x, T y): _x(x), _y(y){};
    Position():_x(0), _y(0){};
};

#endif
