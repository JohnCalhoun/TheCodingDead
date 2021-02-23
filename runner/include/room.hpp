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
    vector<output> interact(map<string, vector<ptr> > others);
    vector<Vertex::output>  simulate();
    Room(string id, float width, float height);
};

#endif
