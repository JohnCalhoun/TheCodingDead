#ifndef ROOM_H_INCLUDED 
#define ROOM_H_INCLUDED

#include <vector>
#include "vertex.hpp"
#include "position.hpp"

using namespace std;

class Room : public Vertex {
    public:
    class Door {
        public: 
        Position in_position;
        Position out_position;
        Room* destination;
        Door(float in_x, float in_y, float out_x, float out_y, Room* other);
    };
    vector<Door> doors; 

    public:
    const float _width;
    const float _height;

    public:
    vector<output> interact(Vertex::MapToVertexs others);
    vector<Vertex::output>  simulate();
    Room(string id, float width, float height);
    void add_door(float in_x, float in_y, float out_x, float out_y, Room* other);
};

#endif
