#include "position.hpp"
#include <algorithm>
#include  <cmath>

using namespace std;
    
Position::Position(float x, float y): _x(x), _y(y){};

float Position::distance_to(Position &other){
    return hypotf((_x-other._x), (_y - other._y));
}

void Position::move(float angle, float distance){
    _x += distance * sinf(angle);
    _y += distance * cosf(angle);
}

float Position::angle_to(Position &other){
    float x = other._x - _x;
    float y = other._y - _y;
    return atan2f(y, x);
}

void Position::clip(float max_width, float max_height){
    _x = min(max_width, max(0.0f, _x));
    _y = min(max_height, max(0.0f, _y));
}
