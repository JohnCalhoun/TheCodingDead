#ifndef POSITION_H_INCLUDED 
#define POSITION_H_INCLUDED

#include <vector>
#include "vertex.hpp"

using namespace std;

class Position {
    private:
    float _x;
    float _y;

    public:
    Position(float x, float y);

    float distance_to(Position &other);
    float angle_to(Position &other); 
    void move(float angle, float distance);
    void clip(float x_max, float y_max);
};

#endif
