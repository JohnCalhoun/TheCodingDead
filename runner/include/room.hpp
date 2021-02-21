#ifndef ROOM_H_INCLUDED 
#define ROOM_H_INCLUDED

#include <vector>
#include "vertex.hpp"

using namespace std;

class Room : public Vertex {
    private:
    const float _width;
    const float _height;

    public:
    vector<Vertex::output>  _update();
    Room(float width, float height): Vertex("Room"), _width(width), _height(height){};
};

#endif
